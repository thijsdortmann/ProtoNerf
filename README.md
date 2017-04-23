# ProtoNerf

Software needed for the Proto Nerfgun actvity. Includes software for a D1 Mini (Arduino / ESP8266 compatible), and a Node.js server.

## Installation instructions

Clone this repository.

### Node

**Make sure you have Node.js >= 6.1.0 installed!**

* Open the Node folder in a command prompt.
* Run `npm install`
* You're good to go! To run the server, just run `node index.js`

### Arduino

* Make sure you have the following libraries installed
	* ESP8266 boards (see https://github.com/esp8266/Arduino)
	* ArduinoThread (see https://github.com/ivanseidel/ArduinoThread)
	* Adafruit NeoPixel library
	* Adafruit PCD8544 library
	* Adafruit GFX library
* Create `WifiConfig.h` in the Arduino folder
* Insert the following, replacing `network SSID` and `network WPA2 password` with the correct credentials:
```c
#ifndef WIFI_SSID
#define WIFI_SSID "network SSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "network WPA2 password"
#endif```
* Upload to board.