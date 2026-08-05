#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.flush();
    Serial.println("UART 115200 OK");
    Serial.println("SafeHome Start!");
    Serial.println("Waiting for data...");
}

void loop() {
    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 1000) {
        lastPrint = millis();
        Serial.println("System running...");
    }
}