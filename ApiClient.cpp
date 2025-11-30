#include "ApiClient.h"



ApiClient::ApiClient()
  : _sensorId(SENSOR_ID),
    _pin(API_X_PIN),
    _softwareVersion(SOFTWARE_VERSION),
    _doc(),
    _values(_doc.createNestedArray("sensordatavalues"))
{
    // Setze software_version am Anfang
    _doc["software_version"] = _softwareVersion;
}

void ApiClient::preparePayload(float pm10, float pm25) {
    // Leere vorherige Werte, falls schon etwas drin ist
    _values.clear();

    // PM10 → value_type "P1" laut API-Spezifikation :contentReference[oaicite:1]{index=1}
    {
        JsonObject obj = _values.createNestedObject();
        obj["value_type"] = "P1";
        obj["value"] = String(pm10, 2);
    }

    // PM2.5 → value_type "P2"
    {
        JsonObject obj = _values.createNestedObject();
        obj["value_type"] = "P2";
        obj["value"] = String(pm25, 2);
    }
}

String ApiClient::makePayload() {
    String payload;
    serializeJson(_doc, payload);
    return payload;
}

void ApiClient::setHeaders(HTTPClient &http) {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Pin", String(_pin));
    http.addHeader("X-Sensor", _sensorId);
}

bool ApiClient::sendData(float pm10, float pm25) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[API] Kein WLAN: Abbruch sendData");
        return false;
    }

    preparePayload(pm10, pm25);

    HTTPClient http;
    http.begin(_apiUrl);
    setHeaders(http);

    String body = makePayload();
    int code = http.POST(body);

    Serial.printf("[API] POST %s -> Code %d\n", _apiUrl, code);
    if (code > 0) {
        String response = http.getString();
        Serial.println("[API] Antwort: " + response);
        http.end();
        return (code >= 200 && code < 300);
    } else {
        Serial.printf("[API] POST-Fehler, Code: %d\n", code);
        http.end();
        return false;
    }
}
