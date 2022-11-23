#include <Arduino.h>
#include <HTTPClient.h>
#include <rpcWiFi.h>
#include <TFT_eSPI.h>

#include "config.h"

TFT_eSPI tft;

bool firstLoaded = false;

void setup() {
  // Buttons
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  
  // Display
  tft.begin();
  tft.setRotation(1); // 3 landscape
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  digitalWrite(LCD_BACKLIGHT, HIGH);

  // Wifi
  tft.drawString("Connecting..." , 10, 10);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  delay(1000);
  
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Ready", 10, 10);
}

int a = 0;
void loop() {
  tft.drawRect(0,70,100,100,TFT_BLACK);
  tft.drawString("C: " + String(a), 10, 100);
  if (digitalRead(WIO_KEY_C) == LOW) {
    if (WiFi.status() != WL_CONNECTED) {
      displayWifiError();
      return;
    }
    
    firstLoaded = false;
    displayFetching();
    
    loadCurrency(PLN_USD_ENDPOINT, "USD/PLN", 4, 4, false);
    loadCurrency(PLN_EUR_ENDPOINT, "EUR/PLN", 4, 26, false);
    loadCurrency(BTC_ENDPOINT, "BTC/PLN", 4, 48, true);
  }

  a ++;
  delay(250);
}

void loadCurrency(String endpoint, String label, int xPos, int yPos, bool isBtc) {
    String payload = httpGet(endpoint);
    int startPos = payload.indexOf("cy~") + 3;
    int endPos = payload.indexOf("~", startPos + 1);
    String value = payload.substring(startPos, endPos);
    
    if (!isBtc && value.length() > 4) {
      value = value.substring(0, 4);
    }
  
    displayValue(label, value, xPos, yPos, isBtc);
}

void displayValue(String label, String value, int xPos, int yPos, bool isBtc) {
  if (!firstLoaded) {
    tft.fillScreen(TFT_BLACK);
    firstLoaded = true;
  }
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);  
  tft.drawString(value, xPos, yPos);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY);
  tft.drawString(label, xPos + (isBtc ? 100 : 52), yPos + 8);
}

void displayError(int httpResponseCode, String payload) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);  
    tft.drawString("HTTP " + String(httpResponseCode) + " " + payload, 10, 10);
}

void displayWifiError() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);  
    tft.drawString("WiFi error", 105, 112);
}

void displayFetching() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);  
    tft.drawString("Fetching..", 105, 112);
}

String httpGet(String url) {
  HTTPClient http;
  http.begin(url);

  String payload;
  int httpResponseCode = http.GET();
  
  if (httpResponseCode >= 200 && httpResponseCode < 300) {
    payload = http.getString();
  } else {
    displayError(httpResponseCode, http.getString());
  }
  
  http.end();
  return payload;
}
