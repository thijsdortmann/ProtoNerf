#include "MenuItem.h"

#define MAINMENU      1
#define IPMENU        2
#define LEDMENU       3
#define COLORMENU     4
#define NAMEMENU      5

#define TEXTSIZE 10
#define OFFSET 15

void handleMenu(uint8_t p) {
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
    case NAMEMENU:
      //do namemenu
      nameMenu(p);
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
}
//change menuSize when adding a new one
int menuSize = 4;
MenuItem allMenus[] = {
  MenuItem("SETTINGS", 0, IPMENU),
  MenuItem("BRIGHTNESS", 1, LEDMENU),
  MenuItem("NAME", 2, NAMEMENU),
  MenuItem("LEDSTRIP", 3, COLORMENU)
};
//allMenus[0] = new MenuItem(0, IPMENU, "SETTINGS");
//allMenus[1] = new MenuItem(1, LEDMENU, "BRIGHTNESS");
//allMenus[2] = new MenuItem(2, COLORMENU, "LEDSTRIP");

void mainMenu(uint8_t input) {
  int offSet = 0;
  int startPos = 0;
  int maxLength = menuSize + 1;
  int NORMALLENGTH = menuSize;
  int menuLength = NORMALLENGTH;
  if (!allowColorCustomization()) {
    menuLength--;
  }
  handleCursor(input, menuLength + 1);
  if (hasRole()) {
    maxLength = 4;
    int roleLocation = maxLength * TEXTSIZE;
    if (menuLength < maxLength) {
      roleLocation = menuLength * TEXTSIZE;
    }
    display.setCursor(10, roleLocation);
    display.fillRect(0, roleLocation - 1, 84, 48, 100);
    display.setTextColor(WHITE);
    display.print(getRole());
    //display.print(roleLocation);
  } else {
    maxLength = 5;
  }
  display.setTextColor(BLACK);
  if (cursorLocation > maxLength - 1) {
    // offSet = TEXTSIZE * ((maxLength - 1) - cursorLocation) * -1;
    //startPos = (maxLength - 1) - cursorLocation;
    startPos = cursorLocation - (maxLength - 1);
    offSet = -1 * (startPos * TEXTSIZE);
  }

  drawSelector(cursorLocation, offSet);
  for (int i = startPos; i < startPos + maxLength; i++) {
    if (i < menuLength) {
      allMenus[i].setLocation(i);
      display.setCursor(10, i * TEXTSIZE + offSet);
      display.println(allMenus[i].getName());
    } else {
      display.setCursor(10, menuLength * TEXTSIZE + offSet);
      display.println("<- EXIT");
    }
  }

  if (input == LONGCLICK) {
    for (int i = 0; i < menuLength; i++) {
      if (cursorLocation == allMenus[i].getLocation()) {
        goToMenu(allMenus[i].getDirection());
      }
    }
    if (cursorLocation == menuLength) {
      leaveMenu();
    }
  }
}

void ipMenu(uint8_t input) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, 84, (TEXTSIZE * 4) - 1, 100);
  display.setCursor(0, 1);
  display.print("IP ADDRESS:");
  display.setCursor(0, TEXTSIZE + 1);
  display.print(WiFi.localIP());
  display.setCursor(10, TEXTSIZE * 2);
  display.print("UID:");
  display.setCursor(0, TEXTSIZE * 3);
  display.print(ESP.getChipId());
  display.setCursor(10, TEXTSIZE * 4);
  display.setTextColor(BLACK);
  display.print("<- BACK");

  handleCursor(input, 1);
  drawSelector(cursorLocation, TEXTSIZE * 4);

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
        if (ledBrightness == 250 || ledBrightness == 255) {
          ledTimer = millis() + 500;
        } else {
          ledTimer = millis();
        }
        ledBrightness += 5;
        ledBrightness %= 260;
      }
    } else if (cursorLocation == 1) {
      sightLedOn = !sightLedOn;
      resetButton();
    } else if (cursorLocation == 2) {
      if (millis() > ledTimer + SCROLLSPEED * 3) {
        if (backLightBrightness == 250 || backLightBrightness == 255) {
          ledTimer = millis() + 500;
        } else {
          ledTimer = millis();
        }
        backLightBrightness += 5;
        backLightBrightness %= 260;
      }
    } else if (cursorLocation == 3) {
      backLightOn = !backLightOn;
      resetButton();
    } else if (cursorLocation == 4) {
      EEPROM.write(brightnessAdress, ledBrightness);
      EEPROM.write(brightnessAdress + 1, backLightBrightness);
      EEPROM.write(brightnessAdress + 2, sightLedOn ? 1 : 0);
      EEPROM.write(brightnessAdress + 3, backLightOn ? 1 : 0);
      EEPROM.commit();
      goToMenu(MAINMENU);
    }
  }
}

String primaries[] = {"RED   ", "GREEN ", "BLUE  "};
#define COLORMENULENGTH   5
long colorTimer = millis();

#define MAXBRIGHTNESS 101
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
  display.print("BRIGHT ");
  if (GENERALBRIGHTNESS == 0) {
    display.print("OFF");
  } else {
    display.print(GENERALBRIGHTNESS);
  }
  display.setCursor(10, TEXTSIZE * 4);
  display.println("<- BACK");

  handleCursor(input, COLORMENULENGTH);
  drawSelector(cursorLocation, 0);
  if (input == LONGCLICK) {
    if (cursorLocation < 3) {
      if (millis() > colorTimer + SCROLLSPEED) {
        if (ledStripColor[cursorLocation] + 1 == 256 || ledStripColor[cursorLocation] == 254) {
          colorTimer = millis() + 500;
        } else {
          colorTimer = millis();
        }
        ledStripColor[cursorLocation]++;
        ledStripColor[cursorLocation] %= 256;
        updateStrip();
      }
    }  else if (cursorLocation < 4) {
      if (millis() > colorTimer + SCROLLSPEED) {
        if (GENERALBRIGHTNESS + 1 == MAXBRIGHTNESS || GENERALBRIGHTNESS == MAXBRIGHTNESS - 2) {
          GENERALBRIGHTNESS ++;
          GENERALBRIGHTNESS %= MAXBRIGHTNESS;
          colorTimer = millis() + 500;
        } else {
          GENERALBRIGHTNESS++;
          GENERALBRIGHTNESS %= MAXBRIGHTNESS;
          colorTimer = millis();
        }
      }
      updateStrip();
    } else {
      for (int i = 0; i < 3; i++) {
        EEPROM.write(colorAdress + i + 1, ledStripColor[i]);
      }
      EEPROM.write(colorAdress + 4, GENERALBRIGHTNESS);
      EEPROM.commit();
      goToMenu(MAINMENU);
    }
  }
}
void nameMenu(uint8_t input) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, 84, (TEXTSIZE * 2) - 1, 100);
  display.setCursor(0, 1);
  display.print("NAME:");
  display.setCursor(0, TEXTSIZE + 1);
  display.print(nickname);
  display.setCursor(10, TEXTSIZE * 2);
  display.setTextColor(BLACK);
  display.print("<- BACK");

  handleCursor(input, 1);
  drawSelector(cursorLocation, TEXTSIZE * 2);

  if (input == LONGCLICK) {
    goToMenu(MAINMENU);
  }
}
void handleCursor(int input, int menuLength) {
  if (input == SINGLECLICK) {
    // Serial.println("singleClick spotted");
    cursorLocation++;
    cursorLocation = cursorLocation % menuLength;
  }
}
void drawSelector(uint8_t loc, int startLocation) {
  //show a triangle
  int yPos = (loc * TEXTSIZE) + startLocation + 3;
  display.fillTriangle(0, yPos - 3, 6, yPos, 0, yPos + 3, 255);
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
void startUpMenu() {
  boolean noChoice = true;
  int currentChoice = 1;
  int yHeight = TEXTSIZE * 3 - 1;
  startTimer(0, 5);
  while (getTimeLeft() > 0) {
    int input = getPress();
//    showLoadingAnimation();

    if (input == SINGLECLICK) {
      currentChoice ++;
      currentChoice %= 2;
    } else if (input == LONGCLICK) {
      noChoice = false;
      resetButton();
      break;
    }
    display.clearDisplay();
    display.setCursor(0, TEXTSIZE * 1);
    display.println("CONNECT TO");
    display.setCursor(0, TEXTSIZE * 2);
    display.println("SERVER?");
    if (currentChoice == 0) {
      display.fillRect(20, yHeight, 19, 10, 100);
      display.setCursor(21, TEXTSIZE * 3);
      display.setTextColor(WHITE);
      display.print("YES");
      //46
      display.setCursor(48, TEXTSIZE * 3);
      display.setTextColor(BLACK);
      display.print("NO");
    } else {
      display.setCursor(21, TEXTSIZE * 3);
      display.setTextColor(BLACK);
      display.print("YES");
      //46
      display.fillRect(46, yHeight, 19, 10, 100);
      display.setCursor(50, TEXTSIZE * 3);
      display.setTextColor(WHITE);
      display.print("NO");
    }
    display.setTextColor(BLACK);
    display.setCursor(27, TEXTSIZE * 4);
    display.println(timeLeft());

    delay(50);
    display.display();
  }
  if (noChoice) {
    shouldConnect(false);
  } else {
    shouldConnect(currentChoice == 0 ? true : false);
  }
  for(int i = 0; i < LEDSTRIPLENGTH; i++) {
    setBrightness(i, GENERALBRIGHTNESS);
  }
  showStrip();
}
long toSeconds() {
  return long(millis() / 1000);
}

