#pragma once

void setupMqtt();
void connectMqtt();
void mqttLoop();
void publishMessage(const char* topic, const char* payload);