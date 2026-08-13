#pragma once

#include <stdint.h>

// Hardware
constexpr uint8_t LED_PIN = 2;
constexpr uint8_t LCD_SDA_PIN = 21;
constexpr uint8_t LCD_SCL_PIN = 22;
constexpr uint8_t LCD_I2C_ADDRESS = 0x3F;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// Servo SG90 (ID 2 - Dieu khien cua)
constexpr uint8_t SERVO_PIN         = 14;
constexpr int     SERVO_OPEN_ANGLE  = 90;  // Mo chot cua
constexpr int     SERVO_CLOSE_ANGLE = 0;   // Dong chot cua

// DHT22/DHT11
constexpr uint8_t DHT_PIN = 4;
constexpr uint32_t DHT_READ_INTERVAL_MS = 2000;
constexpr float    TEMP_THRESHOLD      = 30.0f;   // °C
constexpr float    HUMIDITY_THRESHOLD  = 70.0f;   // %

// Fan Motor (not assigned in the required pin map, so use free GPIOs)
constexpr uint8_t FAN_PIN     = 32;
constexpr uint8_t FAN_IN2_PIN = 33;

// RFID MFRC522 (required project pin map)
constexpr uint8_t RFID_SS_PIN   = 5;   // SDA / chip select
constexpr uint8_t RFID_RST_PIN  = 25;
constexpr uint8_t RFID_SCK_PIN  = 18;
constexpr uint8_t RFID_MISO_PIN = 19;
constexpr uint8_t RFID_MOSI_PIN = 23;

// Ultrasonic HC-SR04
constexpr uint8_t ULTRASONIC_TRIG_PIN = 26;
constexpr uint8_t ULTRASONIC_ECHO_PIN = 27;

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

const char* const LED_COMMAND_TOPIC = "safehome/led/set";
const char* const LED_STATE_TOPIC = "safehome/led/state";

// LCD
const char* const LCD_COMMAND = "safehome/lcd/display";

// RFID and ultrasonic
const char* const RFID_SCAN_TOPIC = "safehome/rfid/scan";
const char* const ULTRASONIC_STATE_TOPIC = "safehome/ultrasonic";

// DHT11 topics (ID 6) - dong bo voi Node-RED
const char* const DHT_TEMPERATURE = "safehome/dht/temperature";  // da sua: /temp -> /temperature
const char* const DHT_HUMIDITY    = "safehome/dht/humidity";     // da sua: /hum  -> /humidity
const char* const FAN_COMMAND     = "safehome/fan/set";
const char* const FAN_STATE_TOPIC = "safehome/fan/state";

// Servo topics (ID 2)
const char* const SERVO_COMMAND    = "safehome/servo/set";
const char* const SERVO_STATE_TOPIC = "safehome/servo/state";
