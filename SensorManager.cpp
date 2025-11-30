#include "SensorManager.h"
#include "config.h"
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

HardwareSerial sdsSerial(1);
static Adafruit_BME280 bme;
//SensorManager sensors(SDS_RX_PIN, SDS_TX_PIN, I2C_SDA_PIN, I2C_SCL_PIN);



SensorManager::SensorManager(int sds_rx, int sds_tx, TwoWire &i2c)
: _sds_rx(sds_rx), _sds_tx(sds_tx), _i2c(i2c),
  _idx(0), _pm25(0), _pm10(0), _temp(0), _hum(0), _press(0) {}

void SensorManager::begin() {
    Serial.println("sensor begin");
    
    // bme init: try both addresses
   bool bme_ok = false;
    if (bme.begin(0x76, &_i2c)) {
        bme_ok = true;
    } else if (bme.begin(0x77, &_i2c)) {
        bme_ok = true;
    }

    // Serial2 for SDS011
    sdsSerial.begin(9600, SERIAL_8N1, _sds_rx, _sds_tx);
    _idx = 0;
}

void SensorManager::update() {
    while (sdsSerial.available()) {
        uint8_t b = sdsSerial.read();
        _parseByte(b);
    }

    static unsigned long last = 0;
    if (millis() - last > 2000) {
        last = millis();
        _readBME();
    }
}

void SensorManager::_readBME() {
    // read
    _temp = bme.readTemperature();
    _hum = bme.readHumidity();
    _press = bme.readPressure() / 100.0F;
}

void SensorManager::_parseByte(uint8_t b) {
    if (_idx == 0) {
        if (b == 0xAA) {
            _frame[_idx++] = b;
        }
        return;
    }
    _frame[_idx++] = b;
    if (_idx >= FRAME_LEN) {
        // end byte check
        if (_frame[0] == 0xAA && _frame[1] == 0xC0 && _frame[9] == 0xAB) {
            uint16_t sum = 0;
            for (int i=2;i<=7;i++) sum += _frame[i];
            uint8_t cs = sum % 256;
            if (cs == _frame[8]) {
                uint16_t pm25_raw = _frame[2] + (_frame[3]<<8);
                uint16_t pm10_raw = _frame[4] + (_frame[5]<<8);
                _pm25 = pm25_raw / 10.0f;
                _pm10 = pm10_raw / 10.0f;
                // debug
                // Serial.printf("[SDS] pm25=%.1f pm10=%.1f\n", _pm25, _pm10);
            }
        }
        _idx = 0;
    }
}

float SensorManager::getPM25() { return _pm25; }
float SensorManager::getPM10() { return _pm10; }
float SensorManager::getTemperature() { return _temp; }
float SensorManager::getHumidity() { return _hum; }
float SensorManager::getPressure() { return _press; }
