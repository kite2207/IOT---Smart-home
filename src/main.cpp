#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
#include "dht_sensor.h"
#include "fan_ctrl.h"
#include "wifi_manager.h"
#include "ultrasonic.h"
#include "servo_ctrl.h"         // <<< Servo SG90 (ID 2)

const int ultrasonic_trigPin = 26;
const int ultrasonic_echoPin = 27;
unsigned long ultrasonic_lastMsg = 0;  // Timer MQTT (1s)

void setup() {
    Serial.begin(115200);
    delay(500);

    setupDisplay();
    setupLed();
    setupFan();
    setupDht();
    setupServo();   // ID 2: Servo dieu khien cua

    setupWifi();
    setupMqtt();
}

void loop() {
    keepWifiConnected();
    mqttLoop();

    unsigned long now = millis();

    // Publish ultrasonic len MQTT moi 1 giay
    // Node-RED se lang nghe va ghi Supabase
    if (now - ultrasonic_lastMsg > 1000)
    {
        ultrasonic_lastMsg = now;
        String status = get_ultrasonic_status(ultrasonic_trigPin, ultrasonic_echoPin) ? "Detect" : "Clear";
        mqttPublish("safehome/ultrasonic", status.c_str());
    }

    delay(10);
}
