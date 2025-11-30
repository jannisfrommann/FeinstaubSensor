#ifndef API_CLIENT_H
#define API_CLIENT_H

#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class ApiClient {
public:
    ApiClient();

    // Sende PM-Werte an die API
    bool sendData(float pm10, float pm25);

private:
    String _sensorId;
    int _pin;
    String _softwareVersion;

    StaticJsonDocument<256> _doc;
    JsonArray _values;

    const char * _apiUrl = "https://api.sensor.community/v1/push-sensor-data/";

    void setHeaders(HTTPClient &http);
    String makePayload();
    void preparePayload(float pm10, float pm25);
};

extern ApiClient apiClient;

#endif // API_CLIENT_H
