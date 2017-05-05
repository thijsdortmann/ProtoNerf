void EEPROMHandler() {
    brightnessAdress = colorAdress + 5;
   EEPROM.begin(128);
  if (EEPROM.read(colorAdress) != 1) {
    for (int i = 0; i < 3; i++) {
      EEPROM.write(i + (colorAdress + 1), ledStripColor[i]);
    }
    EEPROM.write(colorAdress + 4, GENERALBRIGHTNESS);
    
    EEPROM.write(brightnessAdress, ledBrightness);
    EEPROM.write(brightnessAdress + 1, backLightBrightness);
    EEPROM.write(brightnessAdress + 2, sightLedOn ? 1 : 0);
    EEPROM.write(brightnessAdress + 3, backLightOn ? 1 : 0);
  }
  //set it as firstStartup
  EEPROM.write(colorAdress, 1);
  EEPROM.commit();

  // Initialize LED strip colors from EEPROM.
  for (int i = 0; i < 3; i++) {
    ledStripColor[i] = EEPROM.read(colorAdress + i + 1);
  }
  GENERALBRIGHTNESS = EEPROM.read(colorAdress + 4);

  //Sight and such
  ledBrightness = EEPROM.read(brightnessAdress);
  backLightBrightness = EEPROM.read(brightnessAdress + 1);
  sightLedOn = EEPROM.read(brightnessAdress + 2) == 1 ? true : false;
  backLightOn = EEPROM.read(brightnessAdress + 3) == 1 ? true : false;

  pinMode(TRIGGERBUTTON, INPUT_PULLUP);
  pinMode(LOGICBUTTON, INPUT_PULLUP);

  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, backLightBrightness);
  pinMode(SIGHTLED, OUTPUT);
  analogWrite(SIGHTLED, ledBrightness);
}

