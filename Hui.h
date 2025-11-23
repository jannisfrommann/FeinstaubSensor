#ifndef HUI_H
#define HUI_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "config.h"


class Hui {
public:
    // Konstruktor: Pins für Encoder (CLK, DT, SW)
    Hui(int enc_clk, int enc_dt, int enc_sw);

    // init display + pins
    void begin();

    // call regelmäßig in loop
    void loop();

    // Werte setzen (wird vom Main/Orchestrator benutzt)
    void setSensorValues(float pm10, float pm25, float temp, float hum, float press);

    // WiFi info -> IP string, mode ("AP" / "STA" / "AP+STA")
    void setWiFiInfo(const String &ip, const String &mode, bool connected);

private:
    // Encoder pins
    int _enc_clk, _enc_dt, _enc_sw;

    // Sensorwerte
    float _pm10;
    float _pm25;
    float _temp;
    float _hum;
    float _press;

    // WiFi
    String _ipAddress;
    String _wifiModeStr;
    bool _wifiConnected;

    // Display
    Adafruit_SH1106G _display = Adafruit_SH1106G(128, 64, &Wire);

    // Menü / Encoder State
    volatile int _menuIndex;
    volatile long _lastEncMicros;
    const unsigned long ENC_DEBOUNCE_US = 2000UL;
    int _lastA;

    // Button debounce
    bool _lastButtonState;
    unsigned long _lastButtonMillis;
    const unsigned long BTN_DEBOUNCE_MS = 50;
    bool _showWifiUntil;
    unsigned long _showWifiUntilMs;
    const unsigned long WIFI_SHOW_MS = 5000UL;

    //SH110X _display = SH110X(128, 64, &Wire);

    // helper
    void drawCurrent();
    void drawWiFi();
};
#endif
