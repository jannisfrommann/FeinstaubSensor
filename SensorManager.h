#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include <Wire.h>


class SensorManager {
public:
    SensorManager(int sds_rx = SDS_RX_PIN, int sds_tx = SDS_TX_PIN, TwoWire &i2c = Wire);
    void begin();
    void update(); // non-blocking, call frequently

    // getters
    float getPM25();
    float getPM10();
    float getTemperature();
    float getHumidity();
    float getPressure();

private:
    int _sds_rx, _sds_tx;
    TwoWire &_i2c;

    // SDS parsing
    static const int FRAME_LEN = 10;
    uint8_t _frame[FRAME_LEN];
    int _idx;

    volatile float _pm25;
    volatile float _pm10;

    // BME
    float _temp;
    float _hum;
    float _press;

    void _parseByte(uint8_t b);
    void _readBME();
};

extern SensorManager sensors;

#endif
