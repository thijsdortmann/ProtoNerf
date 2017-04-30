#include "Arduino.h"
#include "MenuItem.h"

MenuItem::MenuItem(String _name, int _pos, int _dir) {
  Name = _name;
  pos = _pos;
  dir = _dir;
}

int MenuItem::getLocation() {
  return pos;
}
int MenuItem::getDirection() {
  return dir;
}
void MenuItem::setLocation(int _pos) {
  pos = _pos;
}
void MenuItem::setDirection(int _dir) {
  dir = _dir;
}
String MenuItem::getName() {
  return Name;
}
void MenuItem::setname(String _name) {
  Name = _name;
}

