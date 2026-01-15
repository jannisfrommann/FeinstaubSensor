#include "Wire.h"
#include "Hui.h"
#include "config.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Constructor
Hui::Hui(int enc_clk, int enc_dt, int enc_sw, TwoWire &i2c)
  : _enc_clk(enc_clk), _enc_dt(enc_dt), _enc_sw(enc_sw), _i2c(i2c),
    _pm10(0), _pm25(0), _temp(0), _hum(0), _press(0),
    _ipAddress("0.0.0.0"), _wifiModeStr("OFF"), _wifiConnected(false),
    _menuIndex(0), _lastEncMicros(0), _lastA(0),
    _lastButtonState(false), _lastButtonMillis(0),
    _wifiScreenActive(false),      // <<< NEU
    _display(128, 64, &i2c)
{
}

void Hui::begin() {

    // Hardware reset Display
    pinMode(DISPLAY_RESET_PIN, OUTPUT);
    digitalWrite(DISPLAY_RESET_PIN, LOW);
    delay(10);
    digitalWrite(DISPLAY_RESET_PIN, HIGH);
    delay(50);

    if (!_display.begin(OLED_ADDR, true)) {
        Serial.println("[HUI] Display init fehlgeschlagen");
    } else {
        Serial.println("[HUI] Display ok");
    }

    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 0);
    _display.println("HUI gestartet");
    _display.display();

    // encoder pins
    pinMode(_enc_clk, INPUT_PULLUP);
    pinMode(_enc_dt, INPUT_PULLUP);
    pinMode(_enc_sw, INPUT_PULLUP);

    _lastA = digitalRead(_enc_clk);
}

void Hui::setSensorValues(float pm10, float pm25, float temp, float hum, float press) {
    _pm10 = pm10;
    _pm25 = pm25;
    _temp = temp;
    _hum = hum;
    _press = press;
}

void Hui::setWiFiInfo(const String &ip, const String &mode, bool connected) {
    _ipAddress = ip;
    _wifiModeStr = mode;
    _wifiConnected = connected;
}

void Hui::drawCurrent() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setCursor(0,0);

    switch (_menuIndex) {
        case 0:
            _display.println("Feinstaub - PM");
            _display.println("");
            _display.setTextSize(1);
            _display.print("PM2.5:       ");_display.setTextSize(2); _display.println(_pm25,1);
            _display.setTextSize(1);
            _display.print("PM10:        "); _display.setTextSize(2); _display.println(_pm10,1);
            break;

        case 1:
            _display.println("Umwelt");
            _display.println("");
            _display.print("Temp: "); _display.setTextSize(2); _display.print(_temp,1);
            _display.setTextSize(1); _display.println(" C");
            _display.println("");
            _display.print("Hum:  "); _display.setTextSize(2); _display.print(_hum,1);
            _display.setTextSize(1); _display.println(" %");
            break;

        case 2:
            _display.println("Druck");
            _display.println(""); 
            _display.setTextSize(2); 
            _display.print(_press,1); 
            _display.setTextSize(1); 
            _display.println(" hPa");
            break;

        case 3:
            _display.println("Zusammen");
            _display.print("PM2.5:"); _display.print(_pm25,1);
            _display.print(" PM10:"); _display.println(_pm10,1);
            _display.print("T:"); _display.print(_temp,1);
            _display.print("C H:"); _display.print(_hum,1); 
            _display.println("%");
            break;

        default:
            _display.println("Fehler: kein Menu");
            break;
    }
    _display.display();
}

void Hui::drawWiFi() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setCursor(0,0);
    _display.println("WiFi Info");
    _display.print("Mode: "); _display.println(_wifiModeStr);
    _display.print("Status: "); _display.println(_wifiConnected ? "connected" : "not connected");
    _display.print("IP: "); _display.println(_ipAddress);
    _display.display();
}

void Hui::loop() {

    // -------------------------------------------------------
    //    FALL: WiFi-Screen aktiv → nur auf Button warten
    // -------------------------------------------------------
    if (_wifiScreenActive) {
        bool pressed = (digitalRead(_enc_sw) == LOW);

        if (pressed != _lastButtonState && (millis() - _lastButtonMillis) > BTN_DEBOUNCE_MS) {
            _lastButtonMillis = millis();
            _lastButtonState = pressed;

            if (pressed) {
                _wifiScreenActive = false;
                drawCurrent();
                Serial.println("[HUI] WiFi Screen OFF");
            }
        }
        return;  // Encoder gesperrt
    }

    // -------------------------------------------------------
    //    Normaler Menübetrieb
    // -------------------------------------------------------
    static int lastA = HIGH;
    int a = digitalRead(_enc_clk);

    if (lastA == LOW && a == HIGH) {
        int b = digitalRead(_enc_dt);
        if (b == LOW) _menuIndex++;
        else _menuIndex--;

        if (_menuIndex > 3) _menuIndex = 0;
        if (_menuIndex < 0) _menuIndex = 3;
    }

    drawCurrent();
    lastA = a;

    // -------------------------------------------------------
    //    Button toggelt WiFi-Screen
    // -------------------------------------------------------
    bool pressed = (digitalRead(_enc_sw) == LOW);

    if (pressed != _lastButtonState && (millis() - _lastButtonMillis) > BTN_DEBOUNCE_MS) {
        _lastButtonMillis = millis();
        _lastButtonState = pressed;

        if (pressed) {
            _wifiScreenActive = true;
            drawWiFi();
            Serial.println("[HUI] WiFi Screen ON");
        }
    }
}
