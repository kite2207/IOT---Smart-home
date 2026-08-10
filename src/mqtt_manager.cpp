#include "mqtt_manager.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "fan_ctrl.h"
#include "servo_ctrl.h"

SemaphoreHandle_t mqttMutex = nullptr;

namespace {
    WiFiClientSecure espClient;
    PubSubClient mqttClient(espClient);

    void mqttCallback(char* topic, byte* payload, unsigned int length) {
        Serial.print("\n[MQTT] Co tin nhan moi tu topic: ");
        Serial.println(topic);

        String message;
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        
        Serial.print("[MQTT] Noi dung: ");
        Serial.println(message);

        if (strcmp(topic, LED_COMMAND) == 0) {
            if (message == "ON") {
                Serial.println("[LED] BAT");
            } else if (message == "OFF") {
                Serial.println("[LED] TAT");
            }
        }

        // Lenh dieu khien quat thu cong
        if (strcmp(topic, FAN_COMMAND) == 0) {
            if (message == "ON") {
                setFan(true);
                mqttPublish(FAN_STATE_TOPIC, "ON", true);
                Serial.println("[FAN] Thu cong: BAT");
            } else if (message == "OFF") {
                setFan(false);
                mqttPublish(FAN_STATE_TOPIC, "OFF", true);
                Serial.println("[FAN] Thu cong: TAT");
            }
        }

        // Lenh dieu khien servo (ID 2 - mo/dong cua)
        // Node-RED co the gui: goc so (vd "90") hoac "OPEN"/"CLOSE"
        if (strcmp(topic, SERVO_COMMAND) == 0) {
            if (message == "OPEN") {
                setServoAngle(SERVO_OPEN_ANGLE);
                Serial.println("[SERVO] Lenh: MO CUA (90 do)");
            } else if (message == "CLOSE") {
                setServoAngle(SERVO_CLOSE_ANGLE);
                Serial.println("[SERVO] Lenh: DONG CUA (0 do)");
            } else {
                // Thu parse so nguyen (goc tuy chinh 0-180)
                int angle = message.toInt();
                if (angle >= 0 && angle <= 180) {
                    setServoAngle((uint8_t)angle);
                    Serial.printf("[SERVO] Lenh: Goc = %d do\n", angle);
                }
            }
        }
    }
}

void connectMqtt() {
    while (!mqttClient.connected()) {
        printDisplayLine(0, "[MQTT] Connecting");

        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            printDisplayLine(0," Thanh cong!");
            mqttClient.subscribe(LED_COMMAND);
            mqttClient.subscribe(FAN_COMMAND);
            mqttClient.subscribe(SERVO_COMMAND);  // ID 2: Dieu khien cua
        } else {
            printDisplayLine(0," That bai");
            delay(5000);
        }
    }
}

void setupMqtt() {
    mqttMutex = xSemaphoreCreateMutex();
    espClient.setInsecure();
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}

void mqttLoop() {
    if (!mqttClient.connected()) {
        connectMqtt();
    }
    if (mqttMutex && xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        mqttClient.loop();
        xSemaphoreGive(mqttMutex);
    }
}

void mqttPublish(const char* topic, const char* payload, bool retained) {
    if (!mqttMutex) return;
    if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        if (mqttClient.connected()) {
            mqttClient.publish(topic, payload, retained);
        }
        xSemaphoreGive(mqttMutex);
    }
}