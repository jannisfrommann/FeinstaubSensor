#include "LocalWebServer.h"
#include "WifiManager.h"
#include "config.h"
#include <Arduino.h>

extern WifiManager wifi;
//LocalWebServer webServer(80);

LocalWebServer::LocalWebServer(uint16_t port) : _server(port),
    _pm10(NAN), _pm25(NAN), _temp(NAN), _hum(NAN), _press(NAN) {}

void LocalWebServer::begin() {
    _server.on("/", [this](){ handleRoot(); });
    _server.on("/data.json", [this](){ handleJson(); });
    _server.on("/wifi", [this](){ handleWifi(); });
    _server.on("/save", [this](){ handleSave(); });
    _server.begin();
    Serial.println("[HTTP] server started");
}

void LocalWebServer::loop() {
    _server.handleClient();
}

void LocalWebServer::setSensorValues(float pm10, float pm25, float temp, float hum, float press) {
    _pm10 = pm10; _pm25 = pm25; _temp = temp; _hum = hum; _press = press;
}

void LocalWebServer::handleRoot() {
    String html = "<html><head><meta charset='utf-8'><title>FeinstaubSensor</title></head><body>";
    html += "<h2>Sensorstatus</h2>";
    html += "<p><strong>PM2.5:</strong> " + String(_pm25,1) + " µg/m³</p>";
    html += "<p><strong>PM10:</strong> " + String(_pm10,1) + " µg/m³</p>";
    html += "<p><strong>Temp:</strong> " + String(_temp,1) + " °C</p>";
    html += "<p><strong>Hum:</strong> " + String(_hum,1) + " %</p>";
    html += "<p><strong>Press:</strong> " + String(_press,1) + " hPa</p>";
    html += "<p><a href='/data.json'>JSON</a></p>";
    html += "<p><a href='/wifi'>WiFi konfigurieren</a></p>";
    html += "</body></html>";
    _server.send(200, "text/html", html);
}

void LocalWebServer::handleJson() {
    String js = "{";
    js += "\"pm25\":" + String(_pm25,1) + ",";
    js += "\"pm10\":" + String(_pm10,1) + ",";
    js += "\"temp\":" + String(_temp,2) + ",";
    js += "\"hum\":" + String(_hum,1) + ",";
    js += "\"press\":" + String(_press,1);
    js += "}";
    _server.send(200, "application/json", js);
}

void LocalWebServer::handleWifi() {
    String html = "<html><head><meta charset='utf-8'><title>WiFi config</title></head><body>";
    html += "<h2>WiFi konfigurieren</h2>";
    html += "<form action='/save' method='POST'>";
    html += "SSID: <input name='ssid' length=32><br>";
    html += "Passwort: <input name='pass' length=64><br>";
    html += "<input type='submit' value='Speichern & Verbinden'>";
    html += "</form>";
    html += "<p><a href='/'>Zurück</a></p>";
    html += "</body></html>";
    _server.send(200, "text/html", html);
}

void LocalWebServer::handleSave() {
    if (_server.method() != HTTP_POST) {
        _server.send(405, "text/plain", "Method not allowed");
        return;
    }
    String ssid = _server.arg("ssid");
    String pass = _server.arg("pass");

    // suspend AP checks to stop interfering
    wifi.suspendAPCheck(true);
    wifi.saveCredentials(ssid, pass);

    // try to connect
    _server.send(200, "text/html", "<html><body>Versuche, zu verbinden...<br>Wenn erfolgreich, startet das Gerät neu.</body></html>");
    delay(500);
    // attempt connect and then restart in WifiManager upon success or user can reboot
    ESP.restart();
}
