String role;

boolean _hasRole;
boolean gameTimer;
boolean playing;
boolean ColorCustomization;
boolean firingMode;
boolean _allowReloads;
boolean _hasServer = false;
//timer

//timer is mostly in timer tab.
//start a timer with startTimer(int mins, int secs)
//get the time left in seconds with getTimeLeft();
//get the time left as a string with timeLeft();

//setTimer enables the display of the timer, false is no timer visible, true is visible
void shouldConnect(boolean state) {
  _hasServer = state;
}
boolean hasServer() {
  return _hasServer;
}
void setTimer(boolean state) {
  gameTimer = true;
}
boolean hasTimer() {
  return gameTimer;
}

//role, is visible in the menu if there is a role
void disableRole() {
  _hasRole = false;
}
void enableRole(String _role) {
  role = _role;
  _hasRole = true;
}
//returns the name of your role
String getRole() {
  return role;
}
//returns the boolean
boolean hasRole() {
  return _hasRole;
}

//straightforward
//nothing is done yet with the game state
void setGame(boolean state) {
  playing = state;
}
//straightforward
boolean getGameState() {
  return playing;
}
//allow color customization
void setColorCustomization(boolean state) {
  ColorCustomization = state;
}
boolean allowColorCustomization() {
  return ColorCustomization;
}
//determines the state of the relay, false = no shooting, true is shooting
void setFiringMode(boolean state) {
  firingMode = state;
}
boolean getFiringMode() {
  return firingMode;
}
void setReloads(boolean state) {
  _allowReloads = state;
}
boolean allowReloads() {
  return _allowReloads;
}

