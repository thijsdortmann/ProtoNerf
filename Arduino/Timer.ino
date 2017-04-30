//very stable timer, returns as an integer 
int minutes = 0;
int seconds = 0;
long timeCounter = millis();
String timeLeft() {
  String output = "";
  if (minutes == 0 && seconds == 0) {
  } else {
    if (millis() > timeCounter + 1000) {
      timeCounter += 1000;
     //timeCounter = millis();
      Serial.println("tick");
      seconds--;
    }
    if (seconds < 0) {
      seconds = 59;
      minutes--;
    }
  }
  if (minutes < 10) {
    output += '0';
  }
  output += minutes;
  output += ':';
  if (seconds < 10) {
    output += '0';
  }
  output += seconds;
  return output;
}
void startTimer(uint8_t mins, uint8_t secs) {
  gameHasTimer = true;
  minutes = mins;
  seconds = secs;
  timeCounter = millis();
}
//returns the amount of time left in seconds
int getTimeLeft() {
  return (minutes * 60) + seconds;
}

