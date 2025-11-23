#ifndef LOCAL_WEBSERVER_H
#define LOCAL_WEBSERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "config.h"


class LocalWebServer {
public:
    LocalWebServer(uint16_t port = 80);
    void begin();
    void loop();
    void setSensorValues(float pm10, float pm25, float temp, float hum, float press);

private:
    WebServer _server;
    float _pm10, _pm25, _temp, _hum, _press;

    void handleRoot();
    void handleJson();
    void handleWifi();
    void handleSave();
};

extern LocalWebServer webServer;
#endif
