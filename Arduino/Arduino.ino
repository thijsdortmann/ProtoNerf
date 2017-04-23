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

#include <ESP8266WiFi.h>
#include "WifiConfig.h"

#include "SocketIOClient.h"

#include <Adafruit_NeoPixel.h>

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

// LED strip control
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, D4, NEO_GRB + NEO_KHZ800);
uint16_t ledI, ledJ;

// Threads
Thread* thread_socketHandler;
Thread* thread_ledController;

ThreadController threadController;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println("");

  Serial.println("Initializing LED strip");
  strip.begin();
  strip.show();

  Serial.println("Initializing screen.");

  display.begin();

  // Set contrast of display
  display.setContrast(50);

  // Clear display, thus removing the splash screen.
  display.clearDisplay();

  // Show connecting to WiFi status on LCD.
  showPopup("Connecting WiFi...", ssid);
 
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

  // Connection has been established. Show IP on screen.
  display.clearDisplay();
  display.println("Connected to WiFi");
  display.setTextColor(BLACK, WHITE);
  display.println(WiFi.localIP());
  display.display();

  // Delay to allow user to read IP on screen.
  delay(2000);

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

  // Set up threads
  thread_socketHandler = new Thread();
  thread_socketHandler->enabled = true;
  thread_socketHandler->setInterval(100);
  thread_socketHandler->onRun(socketHandler);

  thread_ledController = new Thread();
  thread_ledController->enabled = true;
  thread_ledController->setInterval(25);
  thread_ledController->onRun(ledController);

  // Set up thread controller
  threadController = ThreadController();
  threadController.add(thread_socketHandler);
  threadController.add(thread_ledController);
}

void loop() {
  threadController.run();
}
