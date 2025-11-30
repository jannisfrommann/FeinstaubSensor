#include "HardwareSerial.h"
#include "pins_arduino.h"
#include "SdLogger.h"
#include "config.h"
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

//SdLogger logger(SD_CS_PIN);
RTC_PCF8523 rtc;



SdLogger::SdLogger(int csPin, TwoWire &i2c)
  : _cs(csPin), _ok(false), _i2c(i2c) {}

bool SdLogger::begin() {
  Serial.println("sdlogger begin");

  // SPI (für SD)
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  Serial.println("spi begin");
  delay(50);

  // I2C (für RTC) - I2C immer starten




  if (!rtc.begin(&_i2c)) {
    Serial.println("[RTC] PCF8523 not found");
    rtcOk = false;
  } else {
    Serial.println("[RTC] init ok");
    //if (!rtc.initialized() || rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("[RTC] time adjusted to build time");
    rtcOk = true;

    //}
  }


  // SD initialisieren (wie vorher)
  for (int i = 0; i < 2; i++) {
    if (SD.begin(_cs)) {
      Serial.println("[SD] init ok");
      _ok = true;
      return true;
    } else {
      Serial.println("[SD] init failed");
      _ok = false;
      return false;
    }
  }
  delay(10);
}

void SdLogger::ensureHeader() {
  if (!_ok) return;
  const char *fn = "/feinstaub.csv";
  if (!SD.exists(fn)) {
    File f = SD.open(fn, FILE_WRITE);
    if (f) {
      f.println("timestamp;pm25;pm10;temp;hum;press");
      f.close();
    }
  }
}

void SdLogger::log(const String &line) {
  if (!_ok) {
    // versuch SD erneut zu initialisieren — verwende SPI-Instanz wie in begin()
    if (!SD.begin(_cs, SPI)) return;
    _ok = true;
  }
  File f = SD.open("/feinstaub.csv", FILE_APPEND);
  Serial.println("[SD] Auf SD geloggt");
  if (!f) return;
  f.println(line);
  f.close();
}

String SdLogger::getTimestamp() {
  if (rtcOk) {
    DateTime now = rtc.now();
    char buf[25];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    return String(buf);
  } else {
    // Fallback: Sekunden seit Start (wie vorher, aber als String)
    return String(millis() / 1000);
  }
}
