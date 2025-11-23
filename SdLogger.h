#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include "config.h"


class SdLogger {
public:
    SdLogger(int csPin = SD_CS_PIN);
    bool begin();
    void log(const String &line);
    void ensureHeader();

private:
    int _cs;
    bool _ok;
};

extern SdLogger logger;

#endif
