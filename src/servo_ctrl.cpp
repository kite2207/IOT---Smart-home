// servo_ctrl.cpp
// Dieu khien Servo SG90 qua thu vien ESP32Servo.
//
// Luong ID 2: Node-RED FE bam OPEN/CLOSE hoac nhap goc (0-180)
//   -> MQTT topic safehome/servo/set (nhan lenh goc, vd: "90")
//   -> ESP32 nhan lenh -> xoay servo
//   -> Publish goc hien tai len safehome/servo/state
//
// Wokwi: dung component wokwi-servo, pin GPIO = SERVO_PIN (config.h)

#include "servo_ctrl.h"
#include "config.h"
#include "mqtt_manager.h"

#include <Arduino.h>
#include <ESP32Servo.h>

namespace {
    Servo doorServo;
    uint8_t currentAngle = 0;   // Goc hien tai (0 = dong, 90 = mo)
}  // namespace

// -----------------------------------------------
void setupServo() {
    // Cap phat 4 timer cho ESP32Servo
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    doorServo.setPeriodHertz(50);               // Servo SG90: 50 Hz
    doorServo.attach(SERVO_PIN, 500, 2400);      // min/max pulse us

    // Vi tri mac dinh: dong (0 do)
    setServoAngle(SERVO_CLOSE_ANGLE);
    Serial.println("[SERVO] Da khoi tao. Goc mac dinh: 0 do (DONG).");
}

// -----------------------------------------------
void setServoAngle(uint8_t angle) {
    if (angle > 180) angle = 180;
    currentAngle = angle;
    doorServo.write(angle);

    // Publish goc hien tai len MQTT
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", angle);
    mqttPublish(SERVO_STATE_TOPIC, buf, true);

    Serial.printf("[SERVO] Goc = %d deg\n", angle);
}

// -----------------------------------------------
uint8_t getServoAngle() {
    return currentAngle;
}
