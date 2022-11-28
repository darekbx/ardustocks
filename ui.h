#ifndef UI_h
#define UI_h

#include <TFT_eSPI.h>
#include "storage.h"

class UI {

  private:
    TFT_eSPI tft;

  public:
    void init();
    void clear();
    void drawStats(String text, int x, int y);
    int drawChart(String label, String data, double posX, double posY);
    void displayValue(String label, String value, int xPos, int yPos, bool isBtc);
    void displayError(String message);
    void displayMessage(String message);
    
  private:
    String getChunk(String data, char separator, int index);
    int countChars(String data, char c);
    double getMax(String data, char c);
};

#endif
