#include "config.h"
#include <Arduino.h>
#include "SdLogger.h"
#include "Hui.h"
#include "SensorManager.h"
#include "wifiManager.h"
#include "LocalWebServer.h"
#include "ApiClient.h"


//I2C 
TwoWire I2C1 = TwoWire(1);
TwoWire I2C2 = TwoWire(0);

// Instantiate singletons / global objects (using your pin layout)
SdLogger logger(SD_CS_PIN, I2C1);
Hui hui(ENC_CLK_PIN, ENC_DT_PIN, ENC_SW_PIN, I2C2);
SensorManager sensors(SDS_RX_PIN, SDS_TX_PIN, I2C1);
WifiManager wifi;
LocalWebServer webServer(80);
ApiClient apiClient;


unsigned long lastLog = 0;
const unsigned long LOG_INTERVAL = 10UL * 1000UL;  // 1 min
unsigned long lastApi = 0;
const unsigned long API_INTERVAL = 5UL * 60UL * 1000UL;  // 5 min

void setup() {
  Serial.begin(115200);
  delay(100);


  I2C1.begin(I2C1_SDA_PIN, I2C1_SCL_PIN);
  delay(50);
  I2C2.begin(I2C2_SDA_PIN, I2C2_SCL_PIN);



  Serial.println("=== FeinstaubSensor starting ===");



  // start wifi (loads stored credentials; if none -> AP)
  wifi.begin();
  delay(100);

  if (!logger.begin()) {
    Serial.println("[main] SD init failed");
  } else {
    logger.ensureHeader();
  }

  // sensors
  sensors.begin();
  delay(100);

  // UI
  hui.begin();
  delay(100);

  // webserver
  webServer.begin();
  delay(100);

  // initial push to UI
  hui.setSensorValues(sensors.getPM10(), sensors.getPM25(), sensors.getTemperature(), sensors.getHumidity(), sensors.getPressure());
  hui.setWiFiInfo(wifi.getIP(), (wifi.isConnected() ? String("STA") : String("AP")), wifi.isConnected());
}

void loop() {
  // wifi background
  wifi.loop();

  // webserver
  webServer.loop();

  // sensors update
  sensors.update();

  // get current values
  String timestamp = logger.getTimestamp();
  float pm25 = sensors.getPM25();
  float pm10 = sensors.getPM10();
  float temp = sensors.getTemperature();
  float hum = sensors.getHumidity();
  float press = sensors.getPressure();

  // update web and ui
  webServer.setSensorValues(pm10, pm25, temp, hum, press);
  hui.setSensorValues(pm10, pm25, temp, hum, press);
  hui.setWiFiInfo(wifi.getIP(), (wifi.isConnected() ? String("STA") : String("AP")), wifi.isConnected());

  // logger
  if (millis() - lastLog > LOG_INTERVAL) {
    lastLog = millis();
    String ts = timestamp;
    String line = ts + ";" + String(pm25, 1) + ";" + String(pm10, 1) + ";" + String(temp, 2) + ";" + String(hum, 1) + ";" + String(press, 1);
    logger.log(line);
    Serial.println("[main] " + line);
  }

  // API send (pm-values only)
  if (millis() - lastApi > API_INTERVAL) {
    lastApi = millis();
    apiClient.sendData(pm10, pm25);
  }

  // UI loop
  hui.loop();

  delay(10);
}
