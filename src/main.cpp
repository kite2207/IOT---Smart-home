#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "dht_sensor.h"
#include "fan_ctrl.h"
#include "wifi_manager.h"
#include "ultrasonic.h"

const int ultrasonic_trigPin = 26;
const int ultrasonic_echoPin = 27;
unsigned long ultrasonic_lastMsg = 0;

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();
    setupFan();
    setupDht();

    setupWifi();
    setupMqtt();
}

void loop() {
    keepWifiConnected();
    mqttLoop();

    unsigned long now = millis();
    if (now - ultrasonic_lastMsg > 1000)
    {
        ultrasonic_lastMsg = now;
        String status = get_ultrasonic_status(ultrasonic_trigPin, ultrasonic_echoPin);
        mqttPublish("safehome/ultrasonic", status.c_str());
    }

    delay(10);
}
