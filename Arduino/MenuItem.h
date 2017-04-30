#ifndef MenuItem_h
#define MenuItem_h

#include "Arduino.h"

class MenuItem {
  public:
  MenuItem(String _name, int _pos, int _dir);
  int getLocation();
  int getDirection();
  void setLocation(int _pos);
  void setDirection(int _dir);
  String getName();
  void setname(String _name);
  private:
  int pos;
  int dir;
  String Name;
};

#endif
