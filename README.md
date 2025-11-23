# Feinstaubmesssystem, basierend auf ESP32-C6-DevBoard

📦 Hardwarekomponenten

1) Feinstaubsensor  
Nova PM sensor SDS011 — PM2.5, PM10

| SDS011 Pin | ESP32 Pin | Beschreibung |
| ---------- | --------- | ------------ |
| **TXD**    | **PIN 4** | SDS011 sendet → ESP32 empfängt |
| **RXD**    | **PIN 5** | SDS011 empfängt → ESP32 sendet |
| **VCC (5V)** | **5V**  | Stromversorgung |
| **GND**    | **GND**   | Gemeinsame Masse |

---

2) Luftsensor  
BME280 — Temperatur, Luftfeuchtigkeit, Luftdruck  
I2C Adresse: 0x76

| BME280 Pin | ESP32 Pin | Beschreibung |
| ---------- | --------- | ------------ |
| **SDA**    | **PIN 6** | I2C Daten |
| **SCL**    | **PIN 7** | I2C Clock |
| **VCC (3.3V)** | **3.3V** | Betriebsspannung |
| **GND**    | **GND**   | Masse |

---

3) OLED Display  
Joy-it SSD1306 (I2C)  
I2C Adresse: 0xC3

| OLED Pin | ESP32 Pin | Beschreibung |
| -------- | --------- | ------------ |
| **SDA**  | **PIN 6** | I2C Daten |
| **SCL**  | **PIN 7** | I2C Clock |
| **VCC (3.3V)** | **3.3V** | Betriebsspannung |
| **GND**  | **GND**   | Masse |

---

4) SD Logger  
Adafruit Adalogger FeatherWing (RTC + SD-Slot, PCF8523)

SD-Modul:

| SD-Modul Pin | ESP32 Pin | Beschreibung |
| ------------ | --------- | ------------ |
| **CS**       | **PIN 3**  | Chip Select (SD card) |
| **MOSI**     | **PIN 22** | SPI MOSI |
| **MISO**     | **PIN 23** | SPI MISO |
| **SCK**      | **PIN 21** | SPI Clock |
| **VCC (3.3V)** | **3.3V** | Betriebsspannung |
| **GND**      | **GND**    | Masse |

RTC-Modul (PCF8523):

| RTC-Modul Pin | ESP32 Pin | Beschreibung |
| ------------- | --------- | ------------ |
| **SDA**       | **PIN 6** | I2C Daten |
| **SCL**       | **PIN 7** | I2C Clock |
| **VCC (3.3V)** | **3.3V** | Betriebsspannung (5V möglich, 3.3V empfohlen) |
| **GND**       | **GND**   | Masse |

---

5) Drehencoder (mit Pushbutton)  
KY-040

| Encoder Pin | ESP32 Pin | Beschreibung |
| ----------- | --------- | ------------ |
| **CLK**     | **PIN 2**  | Drehimpuls (Clock) |
| **DT**      | **PIN 11** | Drehimpuls (Data) |
| **SW**      | **PIN 10** | Push-Button |
| **VCC (3.3V)** | **3.3V** | Versorgung |
| **GND**     | **GND**    | Masse |

---
