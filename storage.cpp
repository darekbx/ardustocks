#include <Arduino.h>

#include "config.h"
#include "storage.h"

bool Storage::init() {
  #if DEBUG
    Serial.print("Initializing SD card...");
  #endif
  
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
    #if DEBUG
      Serial.println("initialization failed!");
    #endif
    return false;
  }

  #if DEBUG
    Serial.print("SD card initialized");
  #endif
  
  return true; 
}

bool Storage::addEntry(String fileName, String value) {
  File file = SD.open(fileName, FILE_APPEND);
  if (file) {
    file.print(value + ",");
    file.close();
    return true;
  } else {
    return false;
  }
}

String Storage::loadEntries(String fileName) {
  File file = SD.open(fileName, FILE_READ);
  if (file) {
    String data = "";
    while (file.available()) {
      char c = file.read();
      data += c;
    }
    file.close();
    return data;
  } else {
    return "";
  }
}
