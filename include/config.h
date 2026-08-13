#pragma once

// Hardware
constexpr uint8_t LED_PIN = 2;
constexpr uint8_t LCD_SDA_PIN = 21;
constexpr uint8_t LCD_SCL_PIN = 22;
constexpr uint8_t LCD_I2C_ADDRESS = 0x3F;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// RFID MFRC522
constexpr uint8_t RFID_SS_PIN = 5;
constexpr uint8_t RFID_RST_PIN = 25;
constexpr uint8_t RFID_SCK_PIN = 18;
constexpr uint8_t RFID_MISO_PIN = 19;
constexpr uint8_t RFID_MOSI_PIN = 23;

// Ultrasonic
constexpr uint8_t ultrasonic_trigPin = 26;
constexpr uint8_t ultrasonic_echoPin = 27;

// Optional serial diagnostics
constexpr bool DEBUG_SERIAL = true;

// Wi-Fi
const char* const WIFI_SSID = "Wokwi-GUEST";
const char* const WIFI_PASSWORD = "";

// MQTT
const char* const MQTT_BROKER = "2cdbc656a97a4c3daad987b08d8a473e.s1.eu.hivemq.cloud";
constexpr uint16_t MQTT_PORT = 8883;
const char* const MQTT_USER = "KoKoNut";
const char* const MQTT_PASSWORD = "kkn!@#1236";

// LED
const char* const LED_COMMAND = "safehome/led/set";
const char* const LED_STATE = "safehome/led/state";

// LCD  
const char* const LCD_COMMAND = "safehome/lcd/display";

// RFID
const char* const RFID_SCAN_TOPIC = "safehome/rfid/scan";
