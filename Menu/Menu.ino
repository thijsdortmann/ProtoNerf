#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// These pins are used for the Nokia 5110 LCD:
// D5 - Serial clock out (SCLK)
// D7 - Serial data out (DIN)
// D6 - Data/Command select (D/C)
// D1 - LCD chip select (CE)
// D2 - LCD reset (RST)

// Initialize display.
// We're using hardware SPI (where SCLK and DIN are always on D5 and D7),
// so there aren't included in the arguments

Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);



#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

//END OF DISPLAY SETTINGS
//NON VARIABLES, CHANGE THESE TO FIT WEMOS

#define TRIGGERBUTTON     A0
#define LOGICBUTTON       D3
#define BACKLIGHT         RX
#define SIGHTLED          D8

#define LONGCLICK         3
#define SINGLECLICK       1
#define DOUBLECLICK       2
#define DEBOUNCE          50
#define SCROLLSPEED       50

String IP = "192.168.55.6";

//logic variables
int bullets = 20;
String broadCastMessage;
int menuLength = 0;
int cursorLocation = 0;
int menuLocation = 0;
int ledBrightness = 125;
boolean sightLedOn  = true;
int backLightBrightness = 255;
boolean backLightOn = true;

boolean broadcast = false;
boolean menu = false;
boolean gamePlaying = false;

boolean allowReloads = true;
boolean gameHasTimer = false;
boolean allowColorCustomization = true;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  broadCastMessage = "";

  pinMode(TRIGGERBUTTON, INPUT);
  pinMode(LOGICBUTTON, INPUT_PULLUP);

  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, HIGH);
  pinMode(SIGHTLED, OUTPUT);
  analogWrite(SIGHTLED, ledBrightness);

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

#define MAINMENU      1
#define IPMENU        2
#define LEDMENU       3
#define COLORMENU     4

#define TEXTSIZE 10
#define OFFSET 15

int clickCounter = 0;

void displayScreen() {
  display.clearDisplay();
  int p = getPress();
  if (p == SINGLECLICK) {
    clickCounter++;
  }
  //Serial.print(p);
  //Serial.print("  ");
  //Serial.println(clickCounter);

  if (broadcast) {
    if (buttonPressed(LOGICBUTTON) || millis() > timeOutTimer + TIMEOUTTIME) {
      //timeOutTimer = millis();
      resetButton();
      broadcast = false;
    }
  }
  else if (menu) {
    //p is input, gives either a long press, or a short press
    //do something menu related
   // Serial.println(menuLocation);
    switch (menuLocation) {
      case IPMENU:
      //show IP
      ipMenu(p);
      break;
      case LEDMENU:
        //do led
        ledMenu(p);
        break;
      case COLORMENU:
        //do colorwheel
        colorMenu(p);
        break;
      default:
        mainMenu(p);
        break;
    }

    //timeout timer, don't touch
    if (p != 0) {
      timeOutTimer = millis();
    } else if (millis() > timeOutTimer + TIMEOUTTIME) {
      leaveMenu();
    }
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
          TimeSinceLast = millis() -DOUBLECLICKTIME;
          isReload = true;
        }
      } else {
        bullets --;
      }
       if (!isReload) {
      TimeSinceLast = millis();
       }
      

    } else if (p == LONGCLICK) {
      goToMenu(MAINMENU);
    }
  }

  display.display();
}
void drawSelector(uint8_t loc, int startLocation) {
  //show a triangle
  int yPos = (loc * TEXTSIZE) + startLocation + 3;
  display.fillTriangle(0, yPos - 3, 6, yPos, 0, yPos + 3, 255);
}
#define NORMALLENGTH    3
void mainMenu(uint8_t input) {
  menuLength = NORMALLENGTH;
  int offSet = 0;
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(10, 0);
  //display.print("ip: ");
  display.print("SETTINGS");
  display.setCursor(10, TEXTSIZE);
  display.println("BRIGHTNESS");
  if (allowColorCustomization) {
    menuLength ++;
    offSet += TEXTSIZE;
    display.setCursor(10, TEXTSIZE * 2);
    display.println("COLOR");
  }
  display.setCursor(10, TEXTSIZE * 2 + offSet);
  display.println("<- BACK");

  handleCursor(input, menuLength);
  if (input == LONGCLICK) {
    if (cursorLocation == menuLength-1) {
      leaveMenu();
    } else {
    goToMenu(cursorLocation+2);
      }
    }
  // Serial.println(cursorLocation);
  drawSelector(cursorLocation, 0);
}

void ipMenu(uint8_t input) {
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("IP ADRESS:");
  display.setCursor(10, TEXTSIZE);
  display.print(IP);
  display.setCursor(10, TEXTSIZE * 2);
  display.print("<- BACK");

  handleCursor(input, 1);
  drawSelector(cursorLocation, TEXTSIZE * 2);

  if (input == LONGCLICK) {
    goToMenu(MAINMENU);
  }
}
long ledTimer = millis();

void ledMenu(uint8_t input) {
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("SIGHT:  ");
  display.print(ledBrightness);
  display.setCursor(10, TEXTSIZE);
  display.print(sightLedOn ? "ON" : "OFF");
  display.setCursor(10, TEXTSIZE * 2);
  display.print("Screen: ");
  display.print(backLightBrightness);
  display.setCursor(10, TEXTSIZE * 3);
  display.print(backLightOn ? "ON" : "OFF");
  display.setCursor(10, TEXTSIZE * 4);
  display.print("<- BACK");

  handleCursor(input, 5);
  drawSelector(cursorLocation, 0);

  if (input == LONGCLICK) {
    if (cursorLocation == 0) {
      if (millis() > ledTimer + SCROLLSPEED * 3) {
        ledBrightness+=5;
        ledBrightness %= 255;
        ledTimer = millis();
      }
    } else if (cursorLocation == 1) {
      sightLedOn = !sightLedOn;
      resetButton();
    } else if (cursorLocation == 2) {
      if (millis() > ledTimer + SCROLLSPEED * 3) {
        backLightBrightness+= 5;
        backLightBrightness %= 255;
        ledTimer = millis();
      }
    } else if (cursorLocation == 3) {
        backLightOn = !backLightOn;
        resetButton();
      } else if (cursorLocation == 4) {
        goToMenu(MAINMENU);
      }
  }
}

String primaries[] = {"RED  ", "GREEN", "BLUE "};
int ledStripColor[] = {0, 0, 0};
#define COLORMENULENGTH   4
long colorTimer = millis();

void colorMenu(uint8_t input) {
  display.setTextSize(1);
  for (int i = 0; i < 3; i++) {
    int yPos = 0 + TEXTSIZE * i;
    display.setCursor(10, yPos);
    display.print(primaries[i]);
    display.print(" ");
    display.print(ledStripColor[i]);
  }
  display.setCursor(10, TEXTSIZE * 3);
  display.println("<- BACK");

  handleCursor(input, COLORMENULENGTH);
  drawSelector(cursorLocation, 0);
  if (input == LONGCLICK) {
    if (cursorLocation < 3) {
      if (millis() > colorTimer + SCROLLSPEED) {
        ledStripColor[cursorLocation]++;
        ledStripColor[cursorLocation] %= 255;
        colorTimer = millis();
      }
    } else {
      goToMenu(MAINMENU);
    }
  }
}
void handleCursor(int input, int menuLength) {
  if (input == SINGLECLICK) {
    // Serial.println("singleClick spotted");
    cursorLocation++;
    cursorLocation = cursorLocation % menuLength;
  }
}

void showBroadcast(String message) {
  //show message on screen

  //set timeOutTimer, after 10 seconds it will disappear
  timeOutTimer = millis();
}
void goToMenu(int location) {
  timeOutTimer = millis();
  menu = true;
  menuLocation = location;
  resetButton();
  cursorLocation = 0;
  //Serial.println(location);
}
void leaveMenu() {
  resetButton();
  menu = false;
}

void reload() {
  bullets = 20;
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
}

void drawAmmoCounter() {

}



