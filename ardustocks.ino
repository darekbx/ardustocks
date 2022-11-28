#include <Arduino.h>

#include "config.h"
#include "stocks_api.h"
#include "storage.h"
#include "ui.h"

StocksApi stocksApi;
Storage storage;
UI ui;

bool firstLoaded = false;

void setup() {
  #if DEBUG
    Serial.begin(115200);
    while(!Serial); 
  #endif
  
  // Buttons
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  
  // Display
  ui.init();

  // SD card
  if (!storage.init()) {
    ui.displayError("Unable to initialize SD card");
    return;
  }
  
  String data1 = storage.loadEntries(PLN_USD_FILE);
  ui.drawChart("PLN/USD", data1, 5, 5);
  
  String data2 = storage.loadEntries(PLN_EUR_FILE);
  ui.drawChart("PLN/EUR", data2, 160, 5);
  
  String data3 = storage.loadEntries(BTC_FILE);
  ui.drawChart("BTC", data3, 5, 110);
  
  String data4 = storage.loadEntries(GOLD_FILE);
  ui.drawChart("Gold", data4, 160, 110);
  //return;
  
  // TO REMOVE
  delay(10000);
  
  // Wifi
  ui.displayMessage("Connecting...");
  if (!stocksApi.connect()) {
    ui.displayError("Wifi connection failed");
    return;
  }

  ui.ready();
}

void loop() {
  if (digitalRead(WIO_KEY_C) == LOW) {
    if (stocksApi.isConnected()) {
      ui.displayError("Wifi is not connected");
      return;
    }
    
    firstLoaded = false;
    ui.displayMessage("Fetching...");

    String value;
    
    value = loadCurrency(PLN_USD_ENDPOINT, PLN_USD_FILE, false);
    ui.displayValue("USD/PLN", value, 4, 4, false);
    
    value = loadCurrency(PLN_EUR_ENDPOINT, PLN_EUR_FILE, false);
    ui.displayValue("EUR/PLN", value, 4, 26, false);
    
    value = loadCurrency(BTC_ENDPOINT, BTC_FILE, true);
    ui.displayValue("BTC/PLN", value, 4, 48, true);
    
    value = loadCurrency(GOLD_ENDPOINT, GOLD_FILE, true);
    ui.displayValue("Gold", value, 4, 70, true);
  }

  delay(150);
}

String loadCurrency(String endpoint, String file, bool isBtc) {
    String payload = stocksApi.httpGET(endpoint);
    if (payload == "") {
      ui.displayError("HTTP error");
      return "";
    }
    
    int startPos = payload.indexOf("cy~") + 3;
    int endPos = payload.indexOf("~", startPos + 1);
    String value = payload.substring(startPos, endPos);
    
    if (!isBtc && value.length() > 4) {
      value = value.substring(0, 4);
    }

    storage.addEntry(file, value);
    
    if (!firstLoaded) {
      ui.clear();
      firstLoaded = true;
    }
    
    return value; 
}
