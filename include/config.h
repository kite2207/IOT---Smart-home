#pragma once
#include <Arduino.h>
// Hardware
constexpr uint8_t LED_PIN = 2;
constexpr uint8_t LCD_SDA_PIN = 21;
constexpr uint8_t LCD_SCL_PIN = 22;
constexpr uint8_t LCD_I2C_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// DHT22/DHT11
constexpr uint8_t DHT_PIN = 4;
constexpr uint32_t DHT_READ_INTERVAL_MS = 2000;
constexpr float    TEMP_THRESHOLD      = 30.0f;   // °C
constexpr float    HUMIDITY_THRESHOLD  = 70.0f;   // %

// Fan Motor (LED Simulator on GPIO19)
constexpr uint8_t FAN_PIN     = 19;
constexpr uint8_t FAN_IN2_PIN = 5;

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

const char* const LED_COMMAND = "safehome/led/set";
const char* const LED_STATE = "safehome/led/state";

const char* const DHT_TEMPERATURE = "safehome/dht/temp";
const char* const DHT_HUMIDITY    = "safehome/dht/hum";
const char* const FAN_COMMAND     = "safehome/fan/set";
const char* const FAN_STATE_TOPIC = "safehome/fan/state";
