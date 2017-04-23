/**
   Arduino part of the Nerfgun customization workshop
   (c) 2017 Study Association Proto
   www.proto.utwente.nl
*/

#define ESP8266

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Fonts/TomThumb.h>

#include <ESP8266WiFi.h>
#include "WifiConfig.h"

#include <SocketIOClient.h>

// ----
// Begin of user configuration
// ---

// SSID and password for WiFi can be configured in WifiConfig.h
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Server address and port
char server[] = "192.168.1.115";
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

SocketIOClient socket;
extern String RID;
extern String Rname;
extern String Rcontent;

unsigned long lastCheck = 0;
long interval = 1000;

unsigned long lastPing = 0;
long pingInterval = 25000;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println("");

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
}

void loop() {
  // If connection is dead, reconnect.
  if (!socket.connected()) {
    
    showPopup("Reconnecting", "The server connection was lost. Trying to restore...");
    
    socket.reconnect(server, port);
    delay(2000);
    gameInit();
    
  } else { // Connection is alive.

    if (millis() - lastPing > pingInterval) {
      // Send heartbeat to keep connection alive even if no data is being exchanged.
      socket.heartbeat(0);
    }

    // If it has been a certain time since last check, check if message are received from
    // Socket.io server.
    if (millis() - lastCheck > interval) {
      lastCheck = millis();
      
      if (socket.monitor()) {
        
      }
    }
  }
}