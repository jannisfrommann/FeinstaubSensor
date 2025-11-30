#ifndef CONFIG_H
#define CONFIG_H

// --- Pinbelegung ---
#define SDS_RX_PIN 4
#define SDS_TX_PIN 5

#define I2C1_SDA_PIN 6
#define I2C1_SCL_PIN 7

#define I2C2_SDA_PIN 18
#define I2C2_SCL_PIN 19

#define DISPLAY_RESET_PIN 1

#define ENC_CLK_PIN 2
#define ENC_DT_PIN 11
#define ENC_SW_PIN 10

//SPI
#define SPI_SCK 21
#define SPI_MISO 23
#define SPI_MOSI 22

#define SD_CS_PIN 3

// --- I2C Adresse ---
#define BME280_ADDR 0x76
#define OLED_ADDR 0x3C
#define RTC_ADDR 0x68

// --- API Placeholder ---
#define API_URL "http://example.com/api"

// --- sonstige Konstanten ---
#define AP_SSID "FeinstaubSensor"
#define AP_PASSWORD "12345678"

#define DEFAULT_WIFI_SSID     "jannisWlanIstDasBeste"
#define DEFAULT_WIFI_PASS     "123456789123456789"


// Deine Sensor‑Community / OpenData-Stuttgart Sensor-ID, z. B. "esp32‑12345678"
#define SENSOR_ID "esp32-55555555"

// Virtueller API‑Pin für deinen Sensortyp (SDS011 → Pin 1) laut Sensor.Community Spezifikation :contentReference[oaicite:0]{index=0}
#define API_X_PIN 1

// Software-Version, die du in deinem JSON mitliefern willst
#define SOFTWARE_VERSION "FeinstaubSensor-v1.0"
 

#endif
