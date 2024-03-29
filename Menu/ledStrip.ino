//stored ledstrip colors
uint8_t red[LEDSTRIPLENGTH];
uint8_t green[LEDSTRIPLENGTH];
uint8_t blue[LEDSTRIPLENGTH];
//brightness start at max
uint8_t brightness[LEDSTRIPLENGTH];

//ledstrip colors with individual brightness
uint8_t visibleRed[LEDSTRIPLENGTH];
uint8_t visibleGreen[LEDSTRIPLENGTH];
uint8_t visibleBlue[LEDSTRIPLENGTH];

//sets and individual pixel color
void setColor(uint8_t pixelNum, uint8_t r, uint8_t g, uint8_t b) {
  red[pixelNum] = r;
  green[pixelNum] = g;
  blue[pixelNum] = b;
  ledStrip.setPixelColor(pixelNum, visibleRed[pixelNum], visibleGreen[pixelNum], visibleBlue[pixelNum]);
}
//set individual pixel brightness, checks if different for less CPU usage
void setBrightness(uint8_t pixelNum, uint8_t _b) {
  if (_b != brightness[pixelNum]) {
    brightness[pixelNum] = _b;
    calcBrightness(pixelNum);
  }
}
//shows the strip
void showStrip() {
  for (int i = 0; i < LEDSTRIPLENGTH; i++) {
    ledStrip.setPixelColor(i, visibleRed[i], visibleGreen[i], visibleBlue[i]);
  }
  ledStrip.show();
}
//only has to be called once
void initializeStrip() {
  prepareStrip();
  for (int i = 0; i < LEDSTRIPLENGTH; i++) {
    calcBrightness(i);
  }
}
/**
 * Calculates the brightness of each individual pixel based on mapping
 * R G and B
 * bit CPU intensive but WEMOS can handle it no problem
 */
void calcBrightness(uint8_t pixel) {
  if (brightness[pixel] <= 0) {
    visibleRed[pixel] = visibleGreen[pixel] = visibleBlue[pixel] = 0;
  } else {
    float mult = 1 - ((float)(255 - brightness[pixel]) / 255);

    //float mult = 1.0 - (1.0 / (float)(BRIGHTNESS));
    visibleRed[pixel] = round((float)red[pixel] * mult);
    visibleGreen[pixel] = round((float)green[pixel] * mult);
    visibleBlue[pixel] = round((float)blue[pixel] * mult);
  }
}
//gets the value from either your team color or your own defined color
//if you make animations it will work with both this way
void prepareStrip() {
  if (allowColorCustomization) {
    for (int i = 0; i < LEDSTRIPLENGTH; i++) {
      red[i] = ledStripColor[0];
      green[i] = ledStripColor[1];
      blue[i] = ledStripColor[2];
      brightness[i] = GENERALBRIGHTNESS;
    }
  } else {
    for (int i = 0; i < LEDSTRIPLENGTH; i++) {
      red[i] = teamColor[0];
      green[i] = teamColor[1];
      blue[i] = teamColor[2];
      brightness[i] = GENERALBRIGHTNESS;
    }
  }
}

