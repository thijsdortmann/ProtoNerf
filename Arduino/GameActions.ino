void gameInit() {
  socket.sendEvent("nick", nickname);
}

void parseData(String event, String data) {
  Serial.println("parseData called");
  Serial.print("event: ");
  Serial.println(event);

  if (event == "broadcast") {
    showBroadcast("New broadcast", data);
  }

  if (event == "setRole") {
    enableRole(data);
  }

  if (event == "allowFiringmode") {
    setFiringMode(data == "true" ? true : false);
  }

  if (event == "allowColorcustomization") {
    setColorCustomization(data == "true" ? true : false);
  }

  if (event == "allowReloading") {
    setReloads(data == "true" ? true : false);
  }

  if (event == "startTimer") {
    int minutes = (data.substring(0,1).toInt() * 10) + data.substring(1,2).toInt();
    int seconds = (data.substring(2,3).toInt() * 10) + data.substring(3,4).toInt();
    
    startTimer(minutes, seconds);
  }
}
