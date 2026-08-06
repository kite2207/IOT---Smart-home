#include "mqtt_manager.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "config.h"

#include "display.h"
#include "servo_ctrl.h"

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
        // Lệnh bật/tắt LED
        if (strcmp(topic, LED_COMMAND) == 0) {
            if (message == "ON") {
                Serial.print("ON");
            } else if (message == "OFF") {
                Serial.print("OFF");
            }
        }

        // Lệnh điều khiển servo: nhận số góc 0-180
        if (strcmp(topic, SERVO_COMMAND) == 0) {
            int angle = message.toInt();
            if (angle >= 0 && angle <= 180) {
                setServoAngle((uint8_t)angle);
                // Phản hồi góc thực tế lên topic state
                String stateStr = String(angle);
                mqttClient.publish(SERVO_STATE, stateStr.c_str(), true);
            } else {
                Serial.println("[SERVO] Goc khong hop le (0-180)");
            }
        }
    }  // mqttCallback
}  // namespace



void connectMqtt() {
    while (!mqttClient.connected()) {
        printDisplayLine(0, "[MQTT] Connecting");

        
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            printDisplayLine(0," Thanh cong!");
            // Đăng ký topics
            mqttClient.subscribe(LED_COMMAND);
            mqttClient.subscribe(SERVO_COMMAND);
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