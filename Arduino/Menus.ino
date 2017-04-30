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
  MenuItem("LEDSTRIP", 2, COLORMENU),
  MenuItem("NAME", 3, NAMEMENU)
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
  if (!allowColorCustomization) {
    menuLength--;
  }
  handleCursor(input, menuLength + 1);
  if (hasRole) {
    maxLength = 4;
    int roleLocation = maxLength * TEXTSIZE;
    if (menuLength < maxLength) {
      roleLocation = menuLength * TEXTSIZE;
    }
    display.setCursor(10, roleLocation);
    display.fillRect(0, roleLocation-1, 84, 48, 100);
    display.setTextColor(WHITE);
    display.print(role);
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
  display.fillRect(0, 0, 84, (TEXTSIZE * 2) -1, 100);
  display.setCursor(0, 1);
  display.print("IP ADRESS:");
  display.setCursor(0, TEXTSIZE+1);
  display.print(WiFi.localIP());
  display.setCursor(10, TEXTSIZE * 2);
  display.setTextColor(BLACK);
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
        ledBrightness += 5;
        ledBrightness %= 260;
        ledTimer = millis();
      }
    } else if (cursorLocation == 1) {
      sightLedOn = !sightLedOn;
      resetButton();
    } else if (cursorLocation == 2) {
      if (millis() > ledTimer + SCROLLSPEED * 3) {
        backLightBrightness += 5;
        backLightBrightness %= 260;
        ledTimer = millis();
      }
    } else if (cursorLocation == 3) {
      backLightOn = !backLightOn;
      resetButton();
    } else if (cursorLocation == 4) {
      EEPROM.write(brightnessAdress, ledBrightness);
      EEPROM.write(brightnessAdress+1, backLightBrightness);
      EEPROM.write(brightnessAdress+2, sightLedOn ? 1 : 0);
      EEPROM.write(brightnessAdress+3, backLightOn ? 1 : 0);
      EEPROM.commit();
      goToMenu(MAINMENU);
    }
  }
}

String primaries[] = {"RED  ", "GREEN", "BLUE "};
#define COLORMENULENGTH   5
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
  display.print("BRIGHTNESS  ");
  display.print(GENERALBRIGHTNESS);
  display.setCursor(10, TEXTSIZE * 4);
  display.println("<- BACK");

  handleCursor(input, COLORMENULENGTH);
  drawSelector(cursorLocation, 0);
  if (input == LONGCLICK) {
    if (cursorLocation < 3) {
      if (millis() > colorTimer + SCROLLSPEED) {
        ledStripColor[cursorLocation]++;
        ledStripColor[cursorLocation] %= 256;
        colorTimer = millis();
      }
    }  else if (cursorLocation < 4) {
      if (millis() > colorTimer + SCROLLSPEED) {
        GENERALBRIGHTNESS++;
        GENERALBRIGHTNESS %= 256;
        colorTimer = millis();
      }
    } else {
      for (int i = 0; i < 3; i++) {
        EEPROM.write(colorAdress + i + 1, ledStripColor[i]);
      }
      EEPROM.commit();
      goToMenu(MAINMENU);
    }
  }
}
void nameMenu(uint8_t input) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, 84, (TEXTSIZE * 2) -1, 100);
  display.setCursor(0, 1);
  display.print("NAME:");
  display.setCursor(0, TEXTSIZE+1);
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
