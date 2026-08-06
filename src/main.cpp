#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "servo_ctrl.h"
#include "wifi_manager.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();
    setupServo();    // Khởi tạo servo + tạo FreeRTOS task

    setupWifi();
    setupMqtt();
}

void loop() {
    keepWifiConnected();
    mqttLoop();
    delay(10);
}