#include <Arduino.h>

#include "config.h"
#include "stocks_api.h"

bool StocksApi::connect() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  byte attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (attempts++ > 10) {
      return false;
    }
  }
  
  delay(1000);
  return true;
}

bool StocksApi::isConnected() {
  return WiFi.status() != WL_CONNECTED;
}

String StocksApi::httpGET(String url) {
  HTTPClient http;
  http.begin(url);

  String payload;
  int httpResponseCode = http.GET();
  
  if (httpResponseCode >= 200 && httpResponseCode < 300) {
    payload = http.getString();
  } else {
    return "";
  }
  
  http.end();
  return payload;
}
