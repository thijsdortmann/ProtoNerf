void gameInit() {
  socket.sendEvent("nick", nickname);
}

void parseData(char event, String data) {
  Serial.println("parseData called");
  Serial.print("event: ");
  Serial.println(event);
  
  switch (event) {
    case 'p':
      showPopup("New message!", data);
      break;
  }
}
