#ifndef CONFIG_H
#define CONFIG_H

// --- Pinbelegung ---
#define SDS_RX_PIN 5
#define SDS_TX_PIN 4

#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7

#define DISPLAY_RESET_PIN 1

#define ENC_CLK_PIN 2
#define ENC_DT_PIN 11
#define ENC_SW_PIN 10

#define SD_CS_PIN 3

// --- BME280 Adresse ---
#define BME280_ADDR 0x76

// --- API Placeholder ---
#define API_URL "http://example.com/api"

// --- sonstige Konstanten ---
#define AP_SSID "FeinstaubSensor"
#define AP_PASSWORD "12345678"
 

#endif
