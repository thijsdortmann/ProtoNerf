/**
   Arduino part of the Nerfgun customization workshop
   (c) 2017 Study Association Proto
   www.proto.utwente.nl
*/
#define normalSwitch false

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
char server[] = "130.89.190.27";
int port = 3000;

// Nickname for this player, used when gun is offline
String nickname = "";

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

#define TRIGGERBUTTON     RX
#define LOGICBUTTON       TX
#define BACKLIGHT         D3
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

//String role = "TRAITOR";
//String broadCastMessage = "";

// EEPROM adresses
int colorAdress = 0;
int brightnessAdress;
#define NEWCOLOR    1;

// Logic variables
int bullets = MAXBULLETS;
int menuLength = 0;
int cursorLocation = 0;
int menuLocation = 0;
int ledBrightness = 255;
boolean sightLedOn  = true;
int backLightBrightness = 255;
boolean backLightOn = true;
//boolean hasRole = true;

boolean broadcast = false;
boolean menu = false;
//boolean gamePlaying = false;

//boolean allowReloads = true;
//boolean gameHasTimer = false;
//boolean allowColorCustomization = true;

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
int DOUBLECLICKTIME = 350;
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
  delay(10);

  brightnessAdress = colorAdress + 5;

  // Set up EEPROM, initialize values if they aren't there yet.
  EEPROMHandler();
  //pinMode(TRIGGERBUTTON, INPUT_PULLUP);
  //pinMode(LOGICBUTTON, INPUT_PULLUP);

  //Seting up basic values
  setTimer(false); //is there a timer?
  disableRole(); //what is the role?
  setGame(false); //is a game being played?
  setColorCustomization(true); //can you set your own color? (otherwise team colors apply)
  setFiringMode(true); //relay on / off
  setReloads(true); //you can reload TODO: set relay off when no bullets are there
  shouldConnect(false); //first start up boolean
  //setReloads(true); //
  switchRelay(true);

  Serial.println("Initializing LED strip");
  ledStrip.begin();
  initializeStrip();

  Serial.println("Initializing screen.");

  display.begin();

  // Set contrast of display
  display.setContrast(60);

  // Clear display, thus removing the splash screen.
  display.clearDisplay();

  startUpMenu();
  delay(200);
  if (hasServer()) {
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
  }

  // Start timers.
  //startTimer(5, 0);

  // Set up threads
  if (hasServer()) {
    thread_socketHandler = new Thread();
    thread_socketHandler->enabled = hasServer();
    thread_socketHandler->setInterval(50);
    thread_socketHandler->onRun(socketHandler);
  }


  thread_lightHandler = new Thread();
  thread_lightHandler->enabled = true;
  thread_lightHandler->setInterval(25);
  thread_lightHandler->onRun(lightHandler);

  thread_displayHandler = new Thread();
  thread_displayHandler->enabled = true;
  thread_displayHandler->setInterval(50);
  thread_displayHandler->onRun(displayHandler);

  // Set up thread controller
  threadController = ThreadController();
  if (hasServer()) {
    threadController.add(thread_socketHandler);
  }

  threadController.add(thread_lightHandler);
  threadController.add(thread_displayHandler);
}

void loop() {
  threadController.run();
}
