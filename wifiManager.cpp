#include "wifiManager.h"
#include "config.h"
#include <Preferences.h>

//WifiManager wifi; // <-- DEFINIERUNG: NUR HIER! Entferne andere Definitions im .ino

WifiManager::WifiManager()
  : _dns(), apActive(false), suspendAP(false),
    _defaultSsid(""), _defaultPass(""), _haveRuntimeDefault(false),
    _scanning(false), _scanStartMillis(0), _lastScanCount(-1)
{}

void WifiManager::begin() {
  // Try to connect using stored/default creds
  loadCredentials();

  // if still not connected, start AP for configuration
  if (!isConnected()) startAccessPoint();
}

void WifiManager::loop() {
  if (suspendAP) return;
  if (apActive) {
    // placeholder: captive portal handling could go here
  } else {
    // auto reconnect if lost
    if (WiFi.status() != WL_CONNECTED) {
      // attempt reconnect but don't spam logs
      static unsigned long lastTry = 0;
      if (millis() - lastTry > 5000) {
        lastTry = millis();
        Serial.println("[WIFI] Verbindung verloren. Versuche reconnect...");
        WiFi.reconnect();
      }
    }
  }
}

// ----------------- credentials -----------------
void WifiManager::saveCredentials(const String &ssid, const String &pwd) {
  Preferences pref;
  pref.begin("wifi", false);
  pref.putString("ssid", ssid);
  pref.putString("pass", pwd);
  pref.end();
  Serial.println("[WIFI] Credentials gespeichert.");
}

void WifiManager::clearStoredCredentials() {
  Preferences pref;
  pref.begin("wifi", false);
  pref.remove("ssid");
  pref.remove("pass");
  pref.end();
  Serial.println("[WIFI] Gespeicherte Credentials gelöscht.");
}

void WifiManager::setDefaultCredentials(const String &ssid, const String &pwd) {
  _defaultSsid = ssid;
  _defaultPass = pwd;
  _haveRuntimeDefault = (_defaultSsid.length() > 0);
  Serial.printf("[WIFI] Runtime Default gesetzt: SSID='%s'\n", _defaultSsid.c_str());
}

bool WifiManager::isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

String WifiManager::getIP() {
  if (isConnected()) return WiFi.localIP().toString();
  if (apActive) return WiFi.softAPIP().toString();
  return String("0.0.0.0");
}

// ----------------- load / connect -----------------
void WifiManager::loadCredentials() {
  // 1) persistent prefs
  Preferences pref;
  pref.begin("wifi", true);
  String ssid = pref.getString("ssid", "");
  String pass = pref.getString("pass", "");
  pref.end();

  if (ssid.length() > 0) {
    Serial.println("[WIFI] Gefundene gespeicherte Credentials, versuche Verbindung...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    connectToWiFi();
    return;
  }

  // 2) runtime defaults
  if (_haveRuntimeDefault && _defaultSsid.length() > 0) {
    Serial.println("[WIFI] Benutze Runtime-Default-Credentials...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(_defaultSsid.c_str(), _defaultPass.c_str());
    connectToWiFi();
    return;
  }

  // 3) compile-time defaults from config.h
#ifdef DEFAULT_WIFI_SSID
  if (strlen(DEFAULT_WIFI_SSID) > 0) {
    Serial.println("[WIFI] Benutze MACRO DEFAULT_WIFI_CREDENTIALS...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);
    connectToWiFi();
    return;
  }
#endif

  Serial.println("[WIFI] Keine Credentials gefunden. AP wird verwendet.");
}

void WifiManager::connectToWiFi() {
  unsigned long start = millis();
  const unsigned long timeout = 8000; // ms
  while (millis() - start < timeout) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("[WIFI] Verbunden, IP: ");
      Serial.println(WiFi.localIP().toString());
      apActive = false;
      return;
    }
    delay(200);
  }
  Serial.println("[WIFI] Verbindung fehlgeschlagen / Timeout");
}

// ----------------- AP -----------------
void WifiManager::startAccessPoint() {
  String apName = String(AP_SSID);
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(apName.c_str(), AP_PASSWORD);
  apActive = true;
  Serial.print("[WIFI] AP gestartet: ");
  Serial.println(apName);
  Serial.print("[WIFI] AP IP: ");
  Serial.println(WiFi.softAPIP().toString());
}

void WifiManager::suspendAPCheck(bool suspend) {
  suspendAP = suspend;
}

// ----------------- async scan API -----------------

// starts an async scan, non-blocking; returns immediately
void WifiManager::startScanAsync() {
  if (_scanning) return; // already in progress
  _lastScanCount = -1;
  _scanning = true;
  _scanStartMillis = millis();
  // use asynchronous scan to avoid disconnecting STA
  WiFi.scanNetworks(true); // start async scan
  Serial.println("[WIFI] startScanAsync() gestartet");
}

// returns true if results ready (scanComplete() >= 0)
bool WifiManager::isScanDone() {
  if (!_scanning) return false;
  int c = WiFi.scanComplete();
  if (c >= 0) {
    _lastScanCount = c;
    _scanning = false;
    Serial.printf("[WIFI] scan done: %d APs\n", c);
    return true;
  }
  // not ready yet; allow timeout: 10s
  if (millis() - _scanStartMillis > 10000) {
    Serial.println("[WIFI] scan timeout");
    // attempt to stop scan and mark not scanning
    WiFi.scanDelete();
    _scanning = false;
    _lastScanCount = -1;
    return true; // treated as done but results empty
  }
  return false;
}

// returns JSON array string like [{"ssid":"X","rssi":-60,"secure":true},...]
// If no results or error -> returns "[]"
String WifiManager::getScanResultsJSON() {
  String out = "[";
  int n = _lastScanCount;
  if (n <= 0) {
    out += "]";
    return out;
  }
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    bool enc = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    ssid.replace("\"","\\\"");
    out += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + String(rssi) + ",\"secure\":" + String(enc ? "true":"false") + "}";
    if (i < n-1) out += ",";
  }
  out += "]";
  // clear results
  WiFi.scanDelete();
  _lastScanCount = -1;
  return out;
}

// convenience blocking wrapper (calls async + waits up to timeout)
String WifiManager::scanNetworksSync(uint32_t timeoutMs) {
  if (!_scanning) startScanAsync();
  unsigned long start = millis();
  while (!isScanDone()) {
    if (millis() - start > timeoutMs) break;
    delay(50);
    yield();
  }
  return getScanResultsJSON();
}
