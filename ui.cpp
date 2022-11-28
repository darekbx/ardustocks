#include <Arduino.h>

#include "config.h"
#include "ui.h"


void UI::init() {
  tft.begin();
  tft.setRotation(1); // 3 landscape
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  digitalWrite(LCD_BACKLIGHT, HIGH);
}

void UI::clear() {
  tft.fillScreen(TFT_BLACK);
}

int UI::drawChart(String label, String data, double posX, double posY) {
  double width = 150;
  double height = 100;

  tft.fillRect(posX, posY, width, height, TFT_WHITE);

  double minValue = 90000.0;
  double maxValue = 0.0;

  double valuesCount = min(width, countChars(data, ','));

  if (valuesCount == 0) {
    return 0;
  }
  
  double pX = 0;
  double pY = 0;
  double index = 0;
  
  for (index = 0; index < valuesCount; index++) {
    String chunk = getChunk(data, ',', index);
    double value = chunk.toDouble();
    minValue = min(value, minValue);
    maxValue = max(value, maxValue);
  }

  if (minValue > 1000.0) {
    minValue -= 10.0;
  } else {
    minValue -= 0.05;
  }
  if (maxValue > 1000.0) {
    maxValue += 10.0;
  } else {
    maxValue += 0.05;
  }
  
  index = 0;
  double widthRatio = width / (valuesCount - 1);
  double heightRatio = height / (maxValue - minValue);
  double lastValue = 0.0;

  for (index = 0; index < valuesCount; index++) {
    String chunk = getChunk(data, ',', index);
    double value = chunk.toDouble();
    
    if (index == 0) {
      pX = posX;
      pY = height - ((value - minValue) * heightRatio);
      continue; 
    }
    
    double x = posX + index * widthRatio;
    double y = height - ((value - minValue) * heightRatio);

    tft.drawLine(pX, posY + pY, x, posY + y, TFT_RED);
    lastValue = value;
    
    pX = x;
    pY = y;
  }
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(label, posX + 5, posY + 5);
  tft.drawString(String(lastValue), posX + 5, posY + height - 10);

  return valuesCount;
}

void UI::drawStats(String text, int x, int y) {
  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY);  
  tft.drawString(text, x, y);
}

void UI::displayValue(String label, String value, int xPos, int yPos, bool isBtc) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);  
  tft.drawString(value, xPos, yPos);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY);
  tft.drawString(label, xPos + (isBtc ? 100 : 52), yPos + 8);
}

void UI::displayError(String message) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);  
  tft.drawString(message, 10, 10);
}

void UI::displayMessage(String message) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);  
  tft.drawString(message, 10, 10);
}

int UI::countChars(String data, char c) {
  int i, count;
  for (i = 0, count = 0; data[i]; i++)
    count += (data[i] == c);
  return count;
}

String UI::getChunk(String data, char separator, int index) {
  int maxIndex = data.length() - 1;
  int j = 0;
  String chunkVal = "";
  for (int i = 0; i <= maxIndex && j <= index; i++) {
    chunkVal.concat(data[i]);
    if (data[i] == separator) {
      j++;
      if (j > index) {
        chunkVal.trim();
        chunkVal.remove(chunkVal.indexOf(separator), 1); // Remove separator
        return chunkVal;
      }
      chunkVal = "";
    } else if ((i == maxIndex) && (j < index)) {
      chunkVal = "";
      return chunkVal;
    }
  }
}
