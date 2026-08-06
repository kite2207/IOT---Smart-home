#pragma once

// Hardware
constexpr uint8_t LED_PIN    = 2;
constexpr uint8_t SERVO_PIN  = 18;   // GPIO18 -> servo signal
constexpr uint8_t LCD_SDA_PIN = 21;
constexpr uint8_t LCD_SCL_PIN = 22;
constexpr uint8_t LCD_I2C_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// Optional serial diagnostics
constexpr bool DEBUG_SERIAL = false;

// Wi-Fi
const char* const WIFI_SSID = "Wokwi-GUEST";
const char* const WIFI_PASSWORD = "";

// MQTT
const char* const MQTT_BROKER = "2cdbc656a97a4c3daad987b08d8a473e.s1.eu.hivemq.cloud";
constexpr uint16_t MQTT_PORT = 8883;
const char* const MQTT_USER = "KoKoNut";
const char* const MQTT_PASSWORD = "kkn!@#1236";

const char* const LED_COMMAND   = "safehome/led/set";
const char* const LED_STATE     = "safehome/led/state";

const char* const SERVO_COMMAND = "safehome/servo/set";   // nhận 0-180
const char* const SERVO_STATE   = "safehome/servo/state"; // phản hồi góc hiện tại
