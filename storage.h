#ifndef Storage_h
#define Storage_h

#include <SPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"

class Storage {

  public:
    bool init();
    bool addEntry(String fileName, String value);
    String loadEntries(String fileName);
};

#endif
