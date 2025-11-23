#include "config.h"
#include <Arduino.h>
#include "SdLogger.h"
#include "Hui.h"
#include "SensorManager.h"
#include "wifiManager.h"
#include "LocalWebServer.h"
#include "ApiClient.h"

// Instantiate singletons / global objects (using your pin layout)
SdLogger logger(SD_CS_PIN);
Hui hui(ENC_CLK_PIN, ENC_DT_PIN, ENC_SW_PIN);
SensorManager sensors(SDS_RX_PIN, SDS_TX_PIN, I2C_SDA_PIN, I2C_SCL_PIN);
WifiManager wifi;
LocalWebServer webServer(80);
ApiClient apiClient;

unsigned long lastLog = 0;
const unsigned long LOG_INTERVAL = 60UL * 1000UL; // 1 min
unsigned long lastApi = 0;
const unsigned long API_INTERVAL = 5UL * 60UL * 1000UL; // 5 min

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("=== FeinstaubSensor starting ===");

    // SD
    if (!logger.begin()) {
        Serial.println("[main] SD init failed");
    } else {
        logger.ensureHeader();
    }

    // start wifi (loads stored credentials; if none -> AP)
    wifi.begin();

    // sensors
    sensors.begin();

    // UI
    hui.begin();

    // webserver
    webServer.begin();

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
    float pm25 = sensors.getPM25();
    float pm10 = sensors.getPM10();
    float temp = sensors.getTemperature();
    float hum  = sensors.getHumidity();
    float press = sensors.getPressure();

    // update web and ui
    webServer.setSensorValues(pm10, pm25, temp, hum, press);
    hui.setSensorValues(pm10, pm25, temp, hum, press);
    hui.setWiFiInfo(wifi.getIP(), (wifi.isConnected() ? String("STA") : String("AP")), wifi.isConnected());

    // logger
    if (millis() - lastLog > LOG_INTERVAL) {
        lastLog = millis();
        String ts = String(millis() / 1000);
        String line = ts + "," + String(pm25,1) + "," + String(pm10,1) + "," + String(temp,2) + "," + String(hum,1) + "," + String(press,1);
        logger.log(line);
        Serial.println("[main] logged: " + line);
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
