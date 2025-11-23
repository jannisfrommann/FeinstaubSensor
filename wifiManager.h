#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <DNSServer.h>
#include "config.h"


class WifiManager {
public:
    WifiManager();
    void begin();
    void loop();
    bool isConnected();
    String getIP();
    void saveCredentials(const String &ssid, const String &pwd);
    void suspendAPCheck(bool suspend);

private:
    DNSServer dns;
    bool apActive;
    bool suspendAP;
    void loadCredentials();
    void connectToWiFi();
    void startAccessPoint();
};

extern WifiManager wifi;
#endif
