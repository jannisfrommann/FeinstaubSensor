#include "LocalWebServer.h"
#include "wifiManager.h"
#include <WiFi.h>

//LocalWebServer webServer(80); // define global webServer instance (remove other defs in .ino)
extern WifiManager wifi;   // wir greifen auf das Objekt aus der .ino zu


LocalWebServer::LocalWebServer(uint16_t port)
  : _server(port), _pm10(NAN), _pm25(NAN), _temp(NAN), _hum(NAN), _press(NAN) {}

void LocalWebServer::begin() {
  Serial.println("[HTTP] Webserver wird gestartet...");
  _server.on("/", [this]() { handleRoot(); });
  _server.on("/data.json", [this]() { handleJson(); });
  _server.on("/wifi", [this]() { handleWifi(); });
  _server.on("/scan", std::bind(&LocalWebServer::handleScan, this));
  _server.on("/save", std::bind(&LocalWebServer::handleSave, this));
  _server.onNotFound([this]() { _server.send(404, "text/plain", "Not found"); }); // correct capture
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

// Uses WifiManager async-scan. Waits up to ~7s for results (non-destructive).
void LocalWebServer::handleScan() {
  Serial.println("[HTTP] /scan requested -> delegating to WifiManager");
  // Start async scan if not already started
  wifi.startScanAsync();

  // Wait for results but without disconnecting STA. Timeout ~7s
  unsigned long start = millis();
  const unsigned long timeout = 7000;
  while (!wifi.isScanDone() && (millis() - start < timeout)) {
    delay(50);
    yield();
  }

  String json = wifi.getScanResultsJSON(); // returns [] if none
  if (json.length() == 0) json = "[]";
  _server.send(200, "application/json", json);
}

void LocalWebServer::handleWifi() {
  String html =
    "<!doctype html><html><head><meta charset='utf-8'><title>WiFi konfigurieren</title>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'/>"
    "<style>body{font-family:Arial,Helvetica,sans-serif;padding:10px;}label{display:block;margin-top:8px;}select,input{width:100%;padding:8px;}</style>"
    "</head><body>"
    "<h2>WiFi konfigurieren</h2>"
    "<button id='scanBtn'>Netzwerke scannen</button>"
    "<p id='scanStatus'></p>"
    "<form id='wifiForm' action='/save' method='POST'>"
    "  <label for='ssid'>Netzwerk</label>"
    "  <select id='ssid' name='ssid'><option value=''>-- bitte scannen --</option></select>"
    "  <label for='pass'>Passwort</label>"
    "  <input id='pass' name='pass' type='password' placeholder='WPA2 Passwort'>"
    "  <br><br>"
    "  <input type='submit' value='Speichern & Verbinden'>"
    "</form>"
    "<p><a href='/'>Zurück</a></p>"
    "<script>"
    "const scanBtn = document.getElementById('scanBtn');"
    "const status = document.getElementById('scanStatus');"
    "const sel = document.getElementById('ssid');"
    "scanBtn.addEventListener('click', ()=>{"
    "  status.innerText='Scanne... das kann einige Sekunden dauern';"
    "  sel.innerHTML='<option>Scanning...</option>';"
    "  fetch('/scan').then(r=>r.json()).then(list=>{"
    "    sel.innerHTML='';"
    "    if(!list || list.length===0){"
    "      sel.innerHTML = '<option value=\"\">Keine Netzwerke gefunden</option>';"
    "      status.innerText = 'Keine Netzwerke gefunden';"
    "      return;"
    "    }"
    "    list.sort((a,b)=>b.rssi - a.rssi);"
    "    list.forEach(n=>{"
    "      const opt = document.createElement('option');"
    "      opt.value = n.ssid;"
    "      opt.text = (n.ssid || '<hidden>') + ' (' + n.rssi + ' dBm) ' + (n.secure ? '🔒' : '');"
    "      sel.appendChild(opt);"
    "    });"
    "    status.innerText = 'Scan fertig. Wähle ein Netzwerk.';"
    "  }).catch(err=>{"
    "    status.innerText = 'Scan fehlgeschlagen';"
    "    sel.innerHTML = '<option value=\"\">Fehler beim Scannen</option>';"
    "  });"
    "});"
    "</script>"
    "</body></html>";

  _server.send(200, "text/html", html);
}

void LocalWebServer::handleSave() {
  if (_server.method() != HTTP_POST) {
    _server.send(405, "text/plain", "Method not allowed");
    return;
  }
  String ssid = _server.arg("ssid");
  String pass = _server.arg("pass");

  Serial.printf("[HTTP] /save ssid='%s'\n", ssid.c_str());
  wifi.saveCredentials(ssid, pass);

  // Option A: Restart immediately to pick up new credentials (uncomment to enable)
  _server.send(200, "text/html", "<html><body>Credentials saved. Rebooting...</body></html>");
  delay(400);
  ESP.restart();


}
