#include "SdLogger.h"
#include "config.h"
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

//SdLogger logger(SD_CS_PIN);
static RTC_PCF8523 rtc;
static bool rtcInit = false;

SdLogger::SdLogger(int csPin) : _cs(csPin), _ok(false) {}

bool SdLogger::begin() {
    if (SD.begin(_cs)) {
        Serial.println("[SD] init ok");
        _ok = true;
        if (!rtcInit) {
            rtcInit = true;
            if (!rtc.begin()) {
                Serial.println("[RTC] PCF8523 not found");
            } else {
                if (!rtc.initialized() || rtc.lostPower()) {
                    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                }
            }
        }
        return true;
    } else {
        Serial.println("[SD] init failed");
        _ok = false;
        return false;
    }
}

void SdLogger::ensureHeader() {
    if (!_ok) return;
    const char *fn = "/feinstaub.csv";
    if (!SD.exists(fn)) {
        File f = SD.open(fn, FILE_WRITE);
        if (f) {
            f.println("timestamp,pm25,pm10,temp,hum,press");
            f.close();
        }
    }
}

void SdLogger::log(const String &line) {
    if (!_ok) {
        if (!SD.begin(_cs)) return;
        _ok = true;
    }
    File f = SD.open("/feinstaub.csv", FILE_APPEND);
    if (!f) return;
    f.println(line);
    f.close();
}
