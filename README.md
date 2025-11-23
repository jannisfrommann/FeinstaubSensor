Feinstaubmessystem, basierend auf ESP32-C6-DevBoard.
---------------------------------------------------
📦 Hardwarekomponenten

**Feinstaubsensor**
*Nova PM sensor SDS011*
Pm2,5, PM10
| SDS011 Pin | ESP32 Pin         | Beschreibung                   |
| ---------- | ----------------- | ------------------------------ |
| **TXD**    | **PIN 4** | SDS011 sendet → ESP32 empfängt |
| **RXD**    | **PIN 5** | SDS011 empfängt → ESP32 sendet |
| **5V**     | **5V**            | Stromversorgung                |
| **GND**    | **GND**           | Gemeinsame Masse               |



**Luftsensor**
*BME280*
I2C Adresse: 0x76
Temperatur, Luftfeuchtigkeit, Luftdruck
| BME280 Pin       | ESP32 Pin   | Beschreibung |
| --------------- | ----------- | ------------ |
| **VCC**         | **3.3V**    |              |
| **GND**         | **GND**     |              |
| **SCL**         | **PIN 7**  | I2C Clock  |
| **SDA**         | **PIN 6** |   I2C Daten |



**OLED Display**
*Joy-it SSD1306 I2C konfiguration*
I2C Adresse: 0XC3
| OLED Pin | ESP32 Pin   |Beschreibung|
| -------- | ----------- |            |
| **VCC**  | **3.3V**    |            |
| **GND**  | **GND**     |            |
| **SDA**  | **PIN 6** |       I2C Daten        |
| **SCL**  | **PIN 7** |      I2C Clock        |


**SD Logger**
*Adafruit Adalogger FeatherWing, RTC + SD-Slot, PCF8523*
| SD-Modul Pin | ESP32 Pin            | Beschreibung  |
| ------------ | -------------------- | ------------- |
| **CS**       | **PIN 3**           | Chip Select   |
| **MOSI**     | **PIN 22**          | SPI MOSI      |
| **MISO**     | **PIN 23**          | SPI MISO      |
| **SCK**      | **PIN 21**          | SPI Clock     |
| **VCC**      | **3.3V**            |               |
| **GND**      | **GND**             | Masse         |

| RTC-Modul Pin | ESP32 Pin   | Beschreibung                       |
| ---------- | ----------- | ---------------------------------- |
| **VCC**    | **3.3V**    | (5V geht auch, aber 3.3V sauberer) |
| **GND**    | **GND**     | Masse                              |
| **SDA**    | **PIN 6** | I2C Daten                          |
| **SCL**    | **PIN 7** | I2C Clock                          |


**Drehencoder (mit Pushbutton)**
*KY-040*
| Encoder Pin | ESP32 Pin   | Beschreibung     |
| ----------- | ----------- | ---------------- |
| **CLK**     | **PIN 2** |  |
| **DT**      | **PIN 11** | Drehimpuls |
| **SW**      | **PIN 10** | Button |
| **+**       | **3.3V**    | Versorgung       |
| **GND**     | **GND**     | Masse            |



