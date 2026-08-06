#include "mqtt_manager.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "config.h"

#include "display.h"
#include "led.h"

// Sử dụng namespace ẩn để giới hạn phạm vi biến chỉ trong file cpp này
namespace {
    WiFiClientSecure espClient;
    PubSubClient mqttClient(espClient);

    void mqttCallback(char* topic, byte* payload, unsigned int length) {
        Serial.print("\n[MQTT] New message from topic: ");
        Serial.println(topic);

        // Chuyển đổi payload (mảng byte) thành String để dễ xử lý
        String message;
        message.reserve(length);
        for (unsigned int i = 0; i < length; i++) {
            message += static_cast<char>(payload[i]);
        }

        Serial.print("[MQTT] Payload: ");
        Serial.println(message);

        printDisplayLine(1, "[MQTT] New msg");

        // --- Xử lý logic điều khiển tại đây ---
        // Chỉ chấp nhận rõ ràng "true" hoặc "false".
        if (strcmp(topic, LED_COMMAND) == 0) {
            message.trim();

            if (message.equalsIgnoreCase("true")) {
                setLed(true);
            } else if (message.equalsIgnoreCase("false")) {
                setLed(false);
            } else {
                Serial.println("[LED] Invalid command; expected true or false");
                return;
            }

            Serial.print("[LED] State: ");
            Serial.println(ledIsOn() ? "ON" : "OFF");
        } else if (strcmp(topic, LCD_COMMAND) == 0) {
            message.trim();

            if (message.length() == 0) {
                Serial.println("[LCD] Ignoring empty message");
                return;
            }

            // printDisplayLine nhận chuỗi C; c_str() hợp lệ trong phạm vi callback.
            printDisplayLine(0, message.c_str());
            Serial.println("[LCD] Message displayed");
        }
    }
}



void connectMqtt() {
    while (!mqttClient.connected()) {
        Serial.print("\n[MQTT] Connecting to broker...");
        printDisplayLine(0, "[MQTT] Connecting");


        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("\n[MQTT] Connected successfully!");
            printDisplayLine(0,"[MQTT] Connected!");
            // Đăng ký lại tất cả topic sau mỗi lần kết nối lại.
            const bool ledSubscribed = mqttClient.subscribe(LED_COMMAND);
            const bool lcdSubscribed = mqttClient.subscribe(LCD_COMMAND);
            Serial.print("[MQTT] Subscriptions: LED=");
            Serial.print(ledSubscribed ? "OK" : "FAILED");
            Serial.print(", LCD=");
            Serial.println(lcdSubscribed ? "OK" : "FAILED");
        } else {
            Serial.print("\n[MQTT] Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" - Retrying in 5s");
            printDisplayLine(0,"[MQTT] Failed!");
            delay(5000);
        }
    }
}

void setupMqtt() {
    // Bỏ qua xác thực chứng chỉ SSL cho kết nối HiveMQ
    espClient.setInsecure();
    
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}

void mqttLoop() {
    if (!mqttClient.connected()) {
        connectMqtt();
    }
    mqttClient.loop();
}

bool publishMqttMessage(const char* topic, const char* message) {
    if (!mqttClient.connected()) {
        return false;
    }

    return mqttClient.publish(topic, message);
}