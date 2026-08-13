#include "mqtt_manager.h"

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <cstdlib>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "config.h"
#include "display.h"
#include "fan_ctrl.h"
#include "led.h"
#include "servo_ctrl.h"

namespace {
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
SemaphoreHandle_t mqttMutex = nullptr;

bool parseAngle(const String& message, uint8_t& angle) {
    const char* value = message.c_str();
    char* end = nullptr;
    const long parsed = strtol(value, &end, 10);

    if (end == value || *end != '\0' || parsed < 0 || parsed > 180) {
        return false;
    }

    angle = static_cast<uint8_t>(parsed);
    return true;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    message.reserve(length);
    for (unsigned int index = 0; index < length; ++index) {
        message += static_cast<char>(payload[index]);
    }
    message.trim();

    Serial.print("\n[MQTT RX] Topic: ");
    Serial.println(topic);
    Serial.print("[MQTT RX] Payload: ");
    Serial.println(message);
    printDisplayLine(1, "[MQTT] New msg");

    if (strcmp(topic, LED_COMMAND_TOPIC) == 0) {
        bool requestedState = false;
        if (message.equalsIgnoreCase("true") || message.equalsIgnoreCase("ON")) {
            requestedState = true;
        } else if (message.equalsIgnoreCase("false") || message.equalsIgnoreCase("OFF")) {
            requestedState = false;
        } else {
            Serial.println("[LED] Invalid command; expected true/false or ON/OFF");
            return;
        }

        setLed(requestedState);
        mqttPublish(LED_STATE_TOPIC, ledIsOn() ? "true" : "false", true);
        Serial.print("[LED] State: ");
        Serial.println(ledIsOn() ? "ON" : "OFF");
    } else if (strcmp(topic, LCD_COMMAND) == 0) {
        if (message.length() == 0) {
            Serial.println("[LCD] Ignoring empty message");
            return;
        }

        if (printDisplayLine(0, message.c_str())) {
            Serial.println("[LCD] Message displayed");
        } else {
            Serial.println("[LCD] Write failed: LCD was not detected during startup");
        }
    } else if (strcmp(topic, FAN_COMMAND) == 0) {
        bool requestedState = false;
        if (message.equalsIgnoreCase("ON") || message.equalsIgnoreCase("true")) {
            requestedState = true;
        } else if (message.equalsIgnoreCase("OFF") || message.equalsIgnoreCase("false")) {
            requestedState = false;
        } else {
            Serial.println("[FAN] Invalid command; expected ON/OFF or true/false");
            return;
        }

        setFan(requestedState);
        mqttPublish(FAN_STATE_TOPIC, fanIsOn() ? "ON" : "OFF", true);
    } else if (strcmp(topic, SERVO_COMMAND) == 0) {
        uint8_t angle = 0;
        if (message.equalsIgnoreCase("true") || message.equalsIgnoreCase("OPEN")) {
            angle = SERVO_OPEN_ANGLE;
        } else if (message.equalsIgnoreCase("false") || message.equalsIgnoreCase("CLOSE")) {
            angle = SERVO_CLOSE_ANGLE;
        } else if (!parseAngle(message, angle)) {
            Serial.println("[SERVO] Invalid command; expected true/false, OPEN/CLOSE, or 0-180");
            return;
        }

        setServoAngle(angle);
        Serial.printf("[SERVO] Angle: %u degrees (%s)\n", angle,
                      angle == SERVO_OPEN_ANGLE ? "OPEN" : "CLOSE");
    }
}

void connectMqttLocked() {
    while (!mqttClient.connected()) {
        Serial.print("\n[MQTT] Connecting to broker...");
        printDisplayLine(0, "[MQTT] Connecting");

        const String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.println(" connected");
            printDisplayLine(0, "[MQTT] Connected!");

            const bool ledSubscribed = mqttClient.subscribe(LED_COMMAND_TOPIC);
            const bool lcdSubscribed = mqttClient.subscribe(LCD_COMMAND);
            const bool fanSubscribed = mqttClient.subscribe(FAN_COMMAND);
            const bool servoSubscribed = mqttClient.subscribe(SERVO_COMMAND);

            Serial.printf(
                "[MQTT] Subscriptions: LED=%s, LCD=%s, FAN=%s, SERVO=%s\n",
                ledSubscribed ? "OK" : "FAILED",
                lcdSubscribed ? "OK" : "FAILED",
                fanSubscribed ? "OK" : "FAILED",
                servoSubscribed ? "OK" : "FAILED"
            );
        } else {
            Serial.printf(" failed, rc=%d; retrying in 5s\n", mqttClient.state());
            printDisplayLine(0, "[MQTT] Failed!");
            delay(5000);
        }
    }
}
}  // namespace

void setupMqtt() {
    // A recursive mutex is required because PubSubClient invokes mqttCallback()
    // inside loop(), and callbacks publish acknowledgement/state messages.
    mqttMutex = xSemaphoreCreateRecursiveMutex();
    if (!mqttMutex) {
        Serial.println("[MQTT] Failed to create mutex");
    }

    espClient.setInsecure();
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}

void connectMqtt() {
    if (!mqttMutex) {
        return;
    }

    if (xSemaphoreTakeRecursive(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        connectMqttLocked();
        xSemaphoreGiveRecursive(mqttMutex);
    }
}

void mqttLoop() {
    if (!mqttMutex) {
        return;
    }

    if (xSemaphoreTakeRecursive(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        if (!mqttClient.connected()) {
            connectMqttLocked();
        }
        mqttClient.loop();
        xSemaphoreGiveRecursive(mqttMutex);
    }
}

void mqttPublish(const char* topic, const char* payload, bool retained) {
    if (!mqttMutex) {
        return;
    }

    if (xSemaphoreTakeRecursive(mqttMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        if (mqttClient.connected()) {
            mqttClient.publish(topic, payload, retained);
        }
        xSemaphoreGiveRecursive(mqttMutex);
    }
}
