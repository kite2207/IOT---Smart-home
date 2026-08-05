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

void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void reconnect()
{
  while (!client.connected()) {
    if (client.connect("ESP32_SafeHome_Client")) {
      // Thành công
    }
    else
    {
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("SafeHome Start!");
}

void loop() {
    delay(1000);
}