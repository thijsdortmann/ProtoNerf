boolean buttonPressed(uint8_t buttonPin) {
  //Serial.println(digitalRead(buttonPin));
  if (!digitalRead(buttonPin)) {
    return true;
  } else {
    return false;
  }
}

//three press types, double click, single click, long click
//defined as 1, 2, and 3, 0 is no click
//double click is only available outside the menu
long pressTimer = millis();
long longPressTimer = millis();
#define DEBOUNCE 50
#define LONGPRESS 350
boolean notPressed = true;
boolean blocked;

void resetButton() {
  blocked = true;
  notPressed = true;
}

int pressType() {
  int output = 0;
  if (blocked) {
    if (!buttonPressed(LOGICBUTTON)) {
      blocked = false;
      //Serial.println("Blocked");
    }
  } else {
    if (buttonPressed(LOGICBUTTON) && notPressed) {
      pressTimer = millis();
      longPressTimer = millis();
      //Serial.println("FirstPress");
      notPressed = false;
    }
    if (millis() > pressTimer + DEBOUNCE) {
      //Serial.println("past debounceTime");
      //if it is pressed for a very long time
      if (millis() > longPressTimer + LONGPRESS) {
        //Serial.println("longPress");
        //if button is pressed, and has not yet been released
        if (buttonPressed(LOGICBUTTON) && notPressed == false) {
          output = LONGCLICK;
          //Serial.println("longClick");
        }
      } else if (buttonPressed(LOGICBUTTON) && notPressed == false) {
        output = SINGLECLICK;
        //Serial.println("singleClick");
      }
      if (!buttonPressed(LOGICBUTTON) && notPressed == false) {
        //was a bounce
        notPressed = true;
      }
    }
  }
  //Serial.println(output);
  return output;
}

boolean picked = true;
int getPress() {
  int type = pressType();
  if (type == LONGCLICK) {
    //Serial.println("long Press");
    picked = true;
    return LONGCLICK;
  }
  else {
    if (type == SINGLECLICK && picked) {
      picked = false;
    }
    if (buttonPressed(LOGICBUTTON) == false) {
      if (picked == false) {
        picked = true;
        //Serial.println("singleClick");
        return SINGLECLICK;
      }
    }
    return 0;
  }
}

long spamTimer = millis();
boolean triggerReleased = true;
boolean triggerPressed = false;
void ammoCounter() {
  //display the ammo counter, dunno how yet
  if (buttonPressed(TRIGGERBUTTON) && triggerReleased) {
    triggerReleased = false;
    spamTimer = millis();
  }
  if (millis() > spamTimer + DEBOUNCE) {
    triggerPressed = true;
  }
  if (triggerPressed) {
    if (!buttonPressed(TRIGGERBUTTON)) {
      if (bullets > 0) {
        //bullets --;
      }
      triggerPressed = false;
    }
  }
  //display.setTextSize(2);
}
