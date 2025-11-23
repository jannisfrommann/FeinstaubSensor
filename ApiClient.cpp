#include "ApiClient.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//ApiClient apiClient;

void ApiClient::sendData(float pm10, float pm25) {
    if (strlen(API_URL) == 0) return; // no configured endpoint
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(API_URL);
    http.addHeader("Content-Type", "application/json");

    // compose JSON with pm values (pm25, pm10)
    StaticJsonDocument<128> doc;
    doc["pm25"] = pm25;
    doc["pm10"] = pm10;
    String payload;
    serializeJson(doc, payload);

    int code = http.POST(payload);
    Serial.printf("[API] POST %s -> %d\n", API_URL, code);
    http.end();
}
