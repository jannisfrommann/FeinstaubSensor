#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include "config.h"
#include <Wire.h>
#include <WiFi.h>        
#include <WebServer.h>   




class SdLogger {
public:

    SdLogger(int csPin = SD_CS_PIN, TwoWire &i2c = Wire);
    bool begin();
    void log(const String &line);
    bool streamCsv(WiFiClient &client);

    void ensureHeader();
    String getTimestamp();

private:
    TwoWire &_i2c;
    int _cs;
    bool _ok;
    bool rtcOk;
};

extern SdLogger logger;

#endif
