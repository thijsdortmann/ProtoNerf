void socketHandler() {
  // If connection is dead, reconnect.
  if (!socket.connected()) {

    showPopup("Reconnecting", "The server connection was lost. Trying to restore...");

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

void ledController() {
    ledJ++;
    if (ledJ > 255) ledJ = 0;
    for (ledI = 0; ledI < strip.numPixels(); ledI++) {
      strip.setPixelColor(ledI, Wheel(((ledI * 256 / strip.numPixels()) + ledJ) & 255));
    }
    strip.show();
}
