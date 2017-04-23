void gameInit() {
  socket.sendEvent("nick", nickname);
}

/*void parseData(String event, String data) {
  switch (event) {
    case "message":
      showPopup("New message!", data);
      break;
  }
}
*/
