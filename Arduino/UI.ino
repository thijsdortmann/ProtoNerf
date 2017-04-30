void showBroadcast(String title, String message) {
  broadcast = true;
  timeOutTimer = millis();
  
  display.clearDisplay();

  display.setFont(&TomThumb);
  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.drawRect(0, 0, 84, 10, BLACK);
  display.setCursor(0, 7);
  display.print(title);
  
  display.setCursor(0, 17);
  display.print(message);
  
  display.display();

  display.setFont();
}

void showMessage(String title, String message) {  
  display.clearDisplay();

  display.setFont(&TomThumb);
  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.drawRect(0, 0, 84, 10, BLACK);
  display.setCursor(0, 7);
  display.print(title);
  
  display.setCursor(0, 17);
  display.print(message);
  
  display.display();

  display.setFont();
}
