#include <Arduino.h>

#include "config.h"
#include "stocks_api.h"
#include "storage.h"
#include "ui.h"

StocksApi stocksApi;
Storage storage;
UI ui;

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
  
  // Wifi
  ui.displayMessage("Connecting...");
  if (!stocksApi.connect()) {
    ui.displayError("Wifi connection failed");
    return;
  }

  refreshData();
  displayCharts();
}

void loop() {
  if (digitalRead(WIO_KEY_C) == LOW) {
    if (stocksApi.isConnected()) {
      ui.displayError("Wifi is not connected");
      return;
    }

    refreshData();
    displayCharts();
  }

  delay(150);
}

void refreshData() {
  ui.displayMessage("Fetching...");
  loadCurrency(PLN_USD_ENDPOINT, PLN_USD_FILE, false);
  loadCurrency(PLN_EUR_ENDPOINT, PLN_EUR_FILE, false);
  loadCurrency(BTC_ENDPOINT, BTC_FILE, true);
  loadCurrency(GOLD_ENDPOINT, GOLD_FILE, true);
}

void displayCharts() {
  ui.clear();

  String data1 = storage.loadEntries(PLN_USD_FILE);
  int c1 = ui.drawChart("PLN/USD", data1, 5, 5);
  
  String data2 = storage.loadEntries(PLN_EUR_FILE);
  int c2 = ui.drawChart("PLN/EUR", data2, 165, 5);
  
  String data3 = storage.loadEntries(BTC_FILE);
  int c3 = ui.drawChart("BTC", data3, 5, 115);
  
  String data4 = storage.loadEntries(GOLD_FILE);
  int c4 = ui.drawChart("Gold", data4, 165, 115);

  ui.drawStats("Sizes: " + 
    String(c1) + " (" + lengthToSize(data1) + "), " + 
    String(c2) + " (" + lengthToSize(data2) + "), " + 
    String(c3) + " (" + lengthToSize(data3) + "), " + 
    String(c4) + " (" + lengthToSize(data4) + ")", 
    5, 224);
}

String lengthToSize(String data) {
  int length = data.length();
  if (length < 1024) {
    return String(length) + "b";
  } else {
    return String(length / 1024) + "Kb";
  }
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
    
    return value; 
}
