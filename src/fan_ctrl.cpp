// fan_ctrl.cpp
// Điều khiển motor quạt DC qua L298N motor driver.
//
// Cách đấu nối:
//   ESP32 GPIO32 (FAN_PIN)     -> L298N IN1
//   ESP32 GPIO33 (FAN_IN2_PIN) -> L298N IN2
//   L298N ENA                  -> 5V (luôn enable)
//   L298N VCC                  -> 5V, GND -> GND
//   L298N OUT1, OUT2           -> hai đầu motor DC
//
// Logic:
//   Bật quạt  -> IN1=HIGH, IN2=LOW  (motor quay chiều thuận)
//   Tắt quạt  -> IN1=LOW,  IN2=LOW  (motor dừng)

#include "fan_ctrl.h"
#include "config.h"
#include <Arduino.h>

namespace {
    volatile bool fanState = false;
}

void setupFan() {
    pinMode(FAN_PIN,     OUTPUT);
    pinMode(FAN_IN2_PIN, OUTPUT);
    // Khởi đầu tắt
    digitalWrite(FAN_PIN,     LOW);
    digitalWrite(FAN_IN2_PIN, LOW);
    fanState = false;
    Serial.println("[FAN] Da khoi tao L298N motor driver");
}

void setFan(bool on) {
    if (fanState == on) return;
    fanState = on;
    if (on) {
        digitalWrite(FAN_IN2_PIN, LOW);   // IN2 thấp trước
        digitalWrite(FAN_PIN,     HIGH);  // IN1 cao -> motor quay
    } else {
        digitalWrite(FAN_PIN,     LOW);   // IN1 thấp -> motor dừng
        digitalWrite(FAN_IN2_PIN, LOW);
    }
    Serial.print("[FAN] Motor quat: ");
    Serial.println(on ? "BAT (quay)" : "TAT");
}

bool fanIsOn() {
    return fanState;
}
