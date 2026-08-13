#include <Arduino.h>

#include "config.h"
#include "dht_sensor.h"
#include "display.h"
#include "fan_ctrl.h"
#include "led.h"
#include "mqtt_manager.h"
#include "rfid.h"
#include "servo_ctrl.h"
#include "ultrasonic.h"
#include "wifi_manager.h"

namespace {
unsigned long lastUltrasonicPublish = 0;
constexpr unsigned long ULTRASONIC_PUBLISH_INTERVAL_MS = 1000;
}

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();
    setupFan();
    setupServo();
    setupRfid();
    setup_ultrasonic(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);

    setupWifi();
    setupMqtt();
    // The DHT task publishes readings, so MQTT must be initialized first.
    setupDht();
}

void loop() {
    keepWifiConnected();
    mqttLoop();
    rfidLoop();

    const unsigned long now = millis();

    if (now - lastUltrasonicPublish >= ULTRASONIC_PUBLISH_INTERVAL_MS) {
        lastUltrasonicPublish = now;
        const bool detected = get_ultrasonic_status(
            ULTRASONIC_TRIG_PIN,
            ULTRASONIC_ECHO_PIN
        );
        mqttPublish(ULTRASONIC_STATE_TOPIC, detected ? "Detect" : "Clear");
    }

    delay(10);
}
