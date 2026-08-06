#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t mqttMutex;

void setupMqtt();
void connectMqtt();
void mqttLoop();
void mqttPublish(const char* topic, const char* payload, bool retained = false);