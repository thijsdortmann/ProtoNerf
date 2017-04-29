#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "MenuItem.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

// These pins are used for the Nokia 5110 LCD:
// D5 - Serial clock out (SCLK)
// D7 - Serial data out (DIN)
// D6 - Data/Command select (D/C)
// D1 - LCD chip select (CE)
// D2 - LCD reset (RST)

// Initialize display.
// We're using hardware SPI (where SCLK and DIN are always on D5 and D7),
// so there aren't included in the arguments

//NON VARIABLES, CHANGE THESE TO FIT WEMOS

#define TRIGGERBUTTON     A0
#define LOGICBUTTON       D3
#define BACKLIGHT         RX
#define SIGHTLED          D8
#define LEDSTRIP          D4
#define RELAY             D0

//IDENTIFIERS
#define LONGCLICK         3
#define SINGLECLICK       1
#define DOUBLECLICK       2

//BEHAVIOUR
#define DEBOUNCE          50
#define SCROLLSPEED       50

//GUN PARAMETERS

#define MAXBULLETS        20
#define LEDSTRIPLENGTH    10

Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(LEDSTRIPLENGTH, LEDSTRIP, NEO_GRB + NEO_KHZ800);

#define NUMFLAKES 10
#define XPOS      0
#define YPOS      1
#define DELTAY    2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

//END OF DISPLAY SETTINGS

String IP = "192.168.55.6";
String gunName = "ZF-1";

uint8_t GENERALBRIGHTNESS = 255;
uint8_t ledStripColor[] = {0, 0, 0};
uint8_t teamColor[] = {0, 0, 0};

String role;
String broadCastMessage;

//EEPROM adresses
int colorAdress = 0;
int brightnessAdress;
#define NEWCOLOR    1;

//logic variables
int bullets = MAXBULLETS;
int menuLength = 0;
int cursorLocation = 0;
int menuLocation = 0;
int ledBrightness = 125;
boolean sightLedOn  = true;
int backLightBrightness = 255;
boolean backLightOn = true;
boolean hasRole = true;

boolean broadcast = false;
boolean menu = false;
boolean gamePlaying = false;

boolean allowReloads = true;
boolean gameHasTimer = false;
boolean allowColorCustomization = true;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ledStrip.begin();
  broadCastMessage = "";
  role = "TRAITOR";

  brightnessAdress = colorAdress + 4;

  EEPROM.begin(128);
  if (EEPROM.read(colorAdress) == 0) {
    for (int i = 0; i < 3; i++) {
      EEPROM.write(i + (colorAdress + 1), ledStripColor[i]);
    }
    EEPROM.write(brightnessAdress, ledBrightness);
    EEPROM.write(brightnessAdress + 1, backLightBrightness);
    EEPROM.write(brightnessAdress + 2, sightLedOn ? 1 : 0);
    EEPROM.write(brightnessAdress + 3, backLightOn ? 1 : 0);
  }
  EEPROM.write(colorAdress, 1);
  EEPROM.commit();

  for (int i = 0; i < 3; i++) {
    ledStripColor[i] = EEPROM.read(colorAdress + i + 1);
  }
  ledBrightness = EEPROM.read(brightnessAdress);
  backLightBrightness = EEPROM.read(brightnessAdress + 1);
  sightLedOn = EEPROM.read(brightnessAdress + 2) == 1 ? true : false;
  backLightOn = EEPROM.read(brightnessAdress + 3) == 1 ? true : false;
  //sightLedOn = EEPROM.read(brightnessAdress + 2);
  //backLightOn = EEPROM.read(brightnessAdress + 3);

  pinMode(TRIGGERBUTTON, INPUT_PULLUP);
  pinMode(LOGICBUTTON, INPUT_PULLUP);

  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, backLightBrightness);
  pinMode(SIGHTLED, OUTPUT);
  analogWrite(SIGHTLED, ledBrightness);

  //initialize the ledstrip
   initializeStrip();

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(60);

  //display.display(); // show splashscreen
  //delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  startTimer(5, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  displayScreen();
  handleLights();
}

void showDisplay() {
  // if (noMessage) {
  //   ammoCounter();
  // }

}

#define TIMEOUTTIME     10000
int DOUBLECLICKTIME = 250;
long timeOutTimer = millis();
long TimeSinceLast = millis();

//#define MAINMENU      1
//#define IPMENU        2
//#define LEDMENU       3
//#define COLORMENU     4
//
//#define TEXTSIZE 10
//#define OFFSET 15

int clickCounter = 0;

void displayScreen() {
  display.clearDisplay();
  int p = getPress();
  if (p == SINGLECLICK) {
    clickCounter++;
  }

  if (broadcast) {
    if (buttonPressed(LOGICBUTTON) || millis() > timeOutTimer + TIMEOUTTIME) {
      //timeOutTimer = millis();
      resetButton();
      broadcast = false;
    }
  }
  else if (menu) {
    handleMenu(p);
  } else {
    ammoCounter();

    //ammo counter and time:
    display.setTextSize(5);
    if (bullets < 10) {
      display.setCursor(29, 10);
    } else {
      display.setCursor(14, 10);
    }
    display.println(bullets);

    display.setCursor(27, 0);
    display.setTextSize(1);
    //Change this for the actual timer
    if (gameHasTimer) {
      display.println(timeLeft());
    } else {
      display.println("07:29");
    }
    //button controls:
    if (p == SINGLECLICK) {
      boolean isReload = false;
      if (millis() < TimeSinceLast + DOUBLECLICKTIME) {
        //reload the gun if that is possible
        if (allowReloads) {
          reload();
          TimeSinceLast = millis() - DOUBLECLICKTIME;
          isReload = true;
        }
      } else {
        bullets --;
      }
      if (!isReload) {
        TimeSinceLast = millis();
      }


    } else if (p == LONGCLICK) {
      goToMenu(1);
    }
  }

  display.display();
}


void showBroadcast(String message) {
  //show message on screen

  //set timeOutTimer, after 10 seconds it will disappear
  timeOutTimer = millis();
}

void reload() {
  bullets = MAXBULLETS;
}


void handleLights() {
  //lights of the sight
  if (sightLedOn) {
    analogWrite(SIGHTLED, ledBrightness);
  } else {
    analogWrite(SIGHTLED, 0);
  }
  if (backLightOn) {
    analogWrite(BACKLIGHT, backLightBrightness);
  } else {
    analogWrite(BACKLIGHT, 0);
  }
  //backLight

  //do the ledstrip
  prepareStrip(); //keeps the strip updated, doesn not require much memory
  showStrip();    //shows the strip with the current values

  //OTHER FUNCTIONS THAT ARE POSSIBLE
  //setBrightness(index, brightness); //works for each individual pixel
  //setColor(index, red, green, blue); //works for each individual pixel
}




