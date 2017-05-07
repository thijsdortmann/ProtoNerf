void gameInit() {
  socket.sendEvent("identify", String(ESP.getChipId()));
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
    updateStrip();
  }

  if (event == "allowReloading") {
    setReloads(data == "true" ? true : false);
  }

  if (event == "setNickname") {
    setNickname(data);
  }

  if (event == "startTimer") {
    int minutes = (data.substring(0, 1).toInt() * 10) + data.substring(1, 2).toInt();
    int seconds = (data.substring(2, 3).toInt() * 10) + data.substring(3, 4).toInt();

    startTimer(minutes, seconds);
  }

  if (event == "setTeamColor") {
    teamColor[0] = (data.substring(0, 1).toInt() * 100) + (data.substring(1, 2).toInt() * 10) + data.substring(2, 3).toInt();
    teamColor[1] = (data.substring(3, 4).toInt() * 100) + (data.substring(4, 5).toInt() * 10) + data.substring(5, 6).toInt();
    teamColor[2] = (data.substring(6, 7).toInt() * 100) + (data.substring(7, 8).toInt() * 10) + data.substring(8, 9).toInt();
    updateStrip();
  }

  if (event == "setGameState") {
    setGame(data == "true" ? true : false);
  }
}
