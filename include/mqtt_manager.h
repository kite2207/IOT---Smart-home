#pragma once

void setupMqtt();
void connectMqtt();
void mqttLoop();

// Gửi một message MQTT nếu broker đang kết nối.
bool publishMqttMessage(const char* topic, const char* message);