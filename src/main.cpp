#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "wifi_manager.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();

    setupWifi();
    setupMqtt();
}

void loop() {
    keepWifiConnected();
    mqttLoop();
    delay(10);
}