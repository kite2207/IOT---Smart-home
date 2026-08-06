#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "rfid.h"
#include "wifi_manager.h"
#include "ultrasonic.h"

unsigned long ultrasonic_lastMsg = 0;

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();

    setupWifi();
    setupMqtt();
    setupRfid();
    setup_ultrasonic(ultrasonic_trigPin, ultrasonic_echoPin);
}

void loop() {
    keepWifiConnected();
    mqttLoop();

    unsigned long now = millis();
    if (now - ultrasonic_lastMsg > 1000)
    {
        ultrasonic_lastMsg = now;
        const bool detected = get_ultrasonic_status(ultrasonic_trigPin, ultrasonic_echoPin);
        publishMessage("safehome/ultrasonic", detected ? "true" : "false");
    }

    rfidLoop();
    delay(10);
}
