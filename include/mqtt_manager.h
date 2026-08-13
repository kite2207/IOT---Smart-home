#pragma once

void setupMqtt();
void connectMqtt();
void mqttLoop();
void mqttPublish(const char* topic, const char* payload, bool retained = false);
