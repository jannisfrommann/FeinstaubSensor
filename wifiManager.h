#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>

class WifiManager {
public:
  WifiManager();

  void begin();
  void loop();

  // credentials
  void saveCredentials(const String &ssid, const String &pwd);
  void clearStoredCredentials();
  void setDefaultCredentials(const String &ssid, const String &pwd);

  bool isConnected();
  String getIP();

  // async scan API
  // startScanAsync() triggers an asynchronous scan (non-blocking)
  // isScanDone() returns true when results are ready
  // getScanResultsJSON() returns a JSON array string of results (and clears scan results)
  void startScanAsync();
  bool isScanDone();
  String getScanResultsJSON(); // returns "[]" if none or error

  // convenience: perform synchronous scan with timeout (wrapper)
  String scanNetworksSync(uint32_t timeoutMs = 7000);

  // suspend AP management (used during config flows)
  void suspendAPCheck(bool suspend);

private:
  DNSServer _dns;
  bool apActive;
  bool suspendAP;

  // defaults and runtime defaults
  String _defaultSsid;
  String _defaultPass;
  bool _haveRuntimeDefault;

  // scan state
  bool _scanning;
  unsigned long _scanStartMillis;
  int _lastScanCount;

  // internals
  void loadCredentials();      // try prefs / runtime / macros
  void connectToWiFi();        // blocks up to timeout inside
  void startAccessPoint();
};

extern WifiManager wifi; // single global instance (defined in wifiManager.cpp)

#endif // WIFI_MANAGER_H
