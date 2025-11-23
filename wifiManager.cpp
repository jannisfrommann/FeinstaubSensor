#include "wifiManager.h"
#include "config.h"
#include <WiFi.h>
#include <Preferences.h>

//WifiManager wifi;

WifiManager::WifiManager(): dns(), apActive(false), suspendAP(false) {}

void WifiManager::begin() {
    loadCredentials();
    connectToWiFi();
    if (!isConnected()) startAccessPoint();
}

void WifiManager::loadCredentials() {
    Preferences pref;
    pref.begin("wifi", true);
    String ssid = pref.getString("ssid", "");
    String pass = pref.getString("pass", "");
    pref.end();

    if (ssid.length()>0) {
        Serial.println("[WIFI] Found saved credentials, attempting connect...");
        WiFi.mode(WIFI_MODE_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
    }
}

void WifiManager::connectToWiFi() {
    unsigned long start = millis();
    while (millis() - start < 8000) {
        if (WiFi.status()==WL_CONNECTED) {
            Serial.print("[WIFI] connected: "); Serial.println(WiFi.localIP().toString());
            apActive = false;
            return;
        }
        delay(200);
    }
}

void WifiManager::startAccessPoint() {
    String apName = String(AP_SSID);
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(apName.c_str(), AP_PASSWORD);
    apActive = true;
    Serial.print("[WIFI] AP started: "); Serial.println(apName);
}

void WifiManager::loop() {
    if (suspendAP) return;
    if (apActive) {
        // optionally manage captive portal etc.
    } else {
        // check reconnect
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WIFI] lost, try reconnect");
            WiFi.reconnect();
        }
    }
}

bool WifiManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

String WifiManager::getIP() {
    if (isConnected()) return WiFi.localIP().toString();
    if (apActive) return WiFi.softAPIP().toString();
    return String("0.0.0.0");
}

void WifiManager::saveCredentials(const String &ssid, const String &pwd) {
    Preferences pref;
    pref.begin("wifi", false);
    pref.putString("ssid", ssid);
    pref.putString("pass", pwd);
    pref.end();
    Serial.println("[WIFI] Credentials saved");
}

void WifiManager::suspendAPCheck(bool suspend) {
    suspendAP = suspend;
}
