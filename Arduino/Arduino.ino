/**
   Arduino part of the Nerfgun customization workshop
   (c) 2017 Study Association Proto
   www.proto.utwente.nl
*/

#define ESP8266

#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Fonts/TomThumb.h>

#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include "WifiConfig.h"

#include "SocketIOClient.h"

#include <Adafruit_NeoPixel.h>

#include "MenuItem.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

// ----
// Begin of user configuration
// ---

// SSID and password for WiFi can be configured in WifiConfig.h
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Server address and port
char server[] = "192.168.178.34";
int port = 3000;

// Nickname for this player
String nickname = "ThzD";

// ---
// End of user configuration
// ---

// These pins are used for the Nokia 5110 LCD:
// D5 - Serial clock out (SCLK)
// D7 - Serial data out (DIN)
// D6 - Data/Command select (D/C)
// D1 - LCD chip select (CE)
// D2 - LCD reset (RST)

// Non variables, change these to the fit the WeMos

#define TRIGGERBUTTON     TX
#define LOGICBUTTON       D3
#define BACKLIGHT         RX
#define SIGHTLED          D8
#define LEDSTRIP          D4
#define RELAY             D0

// IDENTIFIERS
#define LONGCLICK         3
#define SINGLECLICK       1
#define DOUBLECLICK       2

// BEHAVIOUR
#define DEBOUNCE          50
#define SCROLLSPEED       50

// GUN PARAMETERS
#define MAXBULLETS        20
#define LEDSTRIPLENGTH    10

#define NUMFLAKES 10
#define XPOS      0
#define YPOS      1
#define DELTAY    2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

String role = "TRAITOR";
String broadCastMessage = "";

// EEPROM adresses
int colorAdress = 0;
int brightnessAdress;
#define NEWCOLOR    1;

// Logic variables
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

uint8_t GENERALBRIGHTNESS = 100;
uint8_t ledStripColor[] = {0, 0, 0};
uint8_t teamColor[] = {0, 0, 0};

// Initialize display.
// We're using hardware SPI (where SCLK and DIN are always on D5 and D7),
// so there aren't included in the arguments
Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);

// SocketIO library
SocketIOClient socket;
extern String RID;
extern String Rname;
extern String Rcontent;

// Variables for global timing
unsigned long lastPing = 0;
long pingInterval = 25000;

#define TIMEOUTTIME     10000
int DOUBLECLICKTIME = 250;
int clickCounter = 0;
long timeOutTimer = millis();
long TimeSinceLast = millis();

// LED strip control
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(10, LEDSTRIP, NEO_GRB + NEO_KHZ800);

// Threads
Thread* thread_socketHandler;
Thread* thread_lightHandler;
Thread* thread_displayHandler;

ThreadController threadController;

void setup() {
  Serial.begin(115200);
  delay(10);

  brightnessAdress = colorAdress + 4;
  
  // Set up EEPROM, initialize values if they aren't there yet.
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

  // Initialize LED strip colors from EEPROM.
  for (int i = 0; i < 3; i++) {
    ledStripColor[i] = EEPROM.read(colorAdress + i + 1);
  }
  ledBrightness = EEPROM.read(brightnessAdress);
  backLightBrightness = EEPROM.read(brightnessAdress + 1);
  sightLedOn = EEPROM.read(brightnessAdress + 2) == 1 ? true : false;
  backLightOn = EEPROM.read(brightnessAdress + 3) == 1 ? true : false;

  pinMode(TRIGGERBUTTON, INPUT_PULLUP);
  pinMode(LOGICBUTTON, INPUT_PULLUP);

  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, backLightBrightness);
  pinMode(SIGHTLED, OUTPUT);
  analogWrite(SIGHTLED, ledBrightness);
  
  Serial.println("");

  Serial.println("Initializing LED strip");
  ledStrip.begin();
  initializeStrip();

  Serial.println("Initializing screen.");

  display.begin();

  // Set contrast of display
  display.setContrast(60);

  // Clear display, thus removing the splash screen.
  display.clearDisplay();

  // Show connecting to WiFi status on LCD.
  showMessage("Connecting WiFi", ssid);
 
  // Kindly ask ESP8266 to connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Check if WiFi is connected, if not add a dot on display to show that the ESP8266
  // is connecting and didn't crash.
  while (WiFi.status() != WL_CONNECTED) {
    display.setCursor(5, 83);
    display.print("_");
    display.display();
    delay(500);
  }

  showMessage("Connecting server", server);
  
  // Establish Socket.io connection to server
  if (!socket.connect(server, port)) {
    display.println("Server connection failed.");
    display.display();
    return;
  }

  if (socket.connected()) {
    display.println("Connected to server");
    display.display();
    gameInit();
  }

  // Start timers.
  startTimer(5, 0);

  // Set up threads
  thread_socketHandler = new Thread();
  thread_socketHandler->enabled = true;
  thread_socketHandler->setInterval(100);
  thread_socketHandler->onRun(socketHandler);

  thread_lightHandler = new Thread();
  thread_lightHandler->enabled = true;
  thread_lightHandler->setInterval(25);
  thread_lightHandler->onRun(lightHandler);

  thread_displayHandler = new Thread();
  thread_displayHandler->enabled = true;
  thread_displayHandler->setInterval(100);
  thread_displayHandler->onRun(displayHandler);

  // Set up thread controller
  threadController = ThreadController();
  threadController.add(thread_socketHandler);
  threadController.add(thread_lightHandler);
  threadController.add(thread_displayHandler);
}

void loop() {
  threadController.run();
}
