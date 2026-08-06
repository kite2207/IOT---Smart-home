#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "rfid.h"
#include "wifi_manager.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();

    setupWifi();
    setupMqtt();
    setupRfid();
}

void loop() {
    keepWifiConnected();
    mqttLoop();
    rfidLoop();
    delay(10);
}