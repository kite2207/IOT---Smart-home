#include "mqtt_manager.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "config.h"

#include "display.h"

// Sử dụng namespace ẩn để giới hạn phạm vi biến chỉ trong file cpp này
namespace {
    WiFiClientSecure espClient;
    PubSubClient mqttClient(espClient);

    void mqttCallback(char* topic, byte* payload, unsigned int length) {
        Serial.print("\n[MQTT] Co tin nhan moi tu topic: ");
        Serial.println(topic);

        // Chuyển đổi payload (mảng byte) thành String để dễ xử lý
        String message;
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        
        Serial.print("[MQTT] Noi dung: ");
        Serial.println(message);

        // --- Xử lý logic điều khiển tại đây ---
        // Ví dụ: Nhận lệnh bật/tắt LED
        if (strcmp(topic, "safehome/led/set") == 0) {
            if (message == "ON") {
                Serial.print("ON");
            } else if (message == "OFF") {
                Serial.print("OFF");
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
            // Đăng ký topic ở đây nếu cần
            mqttClient.subscribe("safehome/led/set");
        } else {
            printDisplayLine(0," That bai");
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

void publishMessage(const char* topic, const char* payload) {
    if (mqttClient.connected()) {
        mqttClient.publish(topic, payload);
    }
}