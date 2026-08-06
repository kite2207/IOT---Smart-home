#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "safehome/frontdoor/ultrasonic";

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = 5;
const int echoPin = 18;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_SafeHome_Client")) {
      // Thành công
    } else {
      delay(5000);
    }
  }
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  return duration / 58.2;
}

#include "config.h"
#include "display.h"
#include "led.h"
#include "mqtt_manager.h"
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
        publishMessage("safehome/ultrasonic", status.c_str());
    }

    delay(10);
}