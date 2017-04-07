void showPopup(String title, String message) {
  display.clearDisplay();
  
  display.setFont(&TomThumb);
  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.drawRect(0, 0, 84, 10, BLACK);
  display.setCursor(5, 7);
  display.print(title);
  
  display.drawRect(0, 10, 84, 38, BLACK);
  display.setCursor(5, 17);
  display.print(message);
  
  display.display();
}

