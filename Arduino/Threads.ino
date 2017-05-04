void socketHandler() {
  // If connection is dead, reconnect.
  if (!socket.connected()) {

    showBroadcast("Reconnecting", "The server connection was lost. Trying to restore...");

    socket.reconnect(server, port);
    delay(2000);
    gameInit();

  } else { // Connection is alive.

    if (millis() - lastPing > pingInterval) {
      lastPing = millis();

      // Send heartbeat to keep connection alive even if no data is being exchanged.
      socket.heartbeat(0);
    }

    // If it has been a certain time since last check, check if message are received from
    // Socket.io server.
    if (socket.monitor()) {
      if (RID.length() > 0) parseData(RID[0], Rcontent);
      RID = "";
      Rcontent = "";
    }
  }
}

void displayHandler() {
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
    display.clearDisplay();
    handleMenu(p);
  } else {
    display.clearDisplay();
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
    if (hasTimer()) {
      display.println(timeLeft());
    } else {
      //print something nice here
      //display.println("07:29");
    }
    //button controls:
    normalButtonHandler(p);
  }

  display.display();
}

void lightHandler() {
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
  //relay
  if (getFiringMode()) {
    digitalWrite(RELAY, LOW);
  } else {
    digitalWrite(RELAY, HIGH);
  }
  

  //do the ledstrip
  prepareStrip(); //keeps the strip updated, does not require much memory
  showStrip();    //shows the strip with the current values

  //OTHER FUNCTIONS THAT ARE POSSIBLE
  //setBrightness(index, brightness); //works for each individual pixel
  //setColor(index, red, green, blue); //works for each individual pixel
}

