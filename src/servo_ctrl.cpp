/**
 * servo_ctrl.cpp
 * Điều khiển servo qua FreeRTOS task độc lập.
 *
 * Kiến trúc:
 *   - setServoAngle() ghi angle vào QueueHandle_t (1 phần tử, ghi đè).
 *   - servoTask() chạy mãi mãi trên Core 1, nhận lệnh từ queue và
 *     viết xung PWM ra pin SERVO_PIN.
 *
 * Lý do dùng queue thay biến toàn cục:
 *   - Tránh race condition giữa MQTT callback (Core 1) và servo task.
 *   - FreeRTOS queue là cơ chế IPC chuẩn, an toàn đa luồng.
 */

#include "servo_ctrl.h"
#include "config.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

namespace {
    Servo servo;
    QueueHandle_t servoQueue = nullptr;  // Queue chứa tối đa 1 lệnh angle
    volatile uint8_t currentAngle = 0;

    // ---------------------------------------------------------------
    // FreeRTOS Task: chạy trên Core 1, stack 2 KB, priority 1
    // ---------------------------------------------------------------
    void servoTask(void* /*pvParameters*/) {
        uint8_t targetAngle = 0;

        for (;;) {
            // Chờ lệnh mới từ queue (block vô hạn)
            if (xQueueReceive(servoQueue, &targetAngle, portMAX_DELAY) == pdTRUE) {
                // Clamp giá trị về [0, 180]
                if (targetAngle > 180) targetAngle = 180;

                currentAngle = targetAngle;
                servo.write(targetAngle);

                Serial.print("[SERVO] Di chuyen den goc: ");
                Serial.println(targetAngle);
            }
        }
    }
}  // namespace

// ---------------------------------------------------------------
// API public
// ---------------------------------------------------------------

void setupServo() {
    // Tạo queue 1 phần tử kiểu uint8_t
    servoQueue = xQueueCreate(1, sizeof(uint8_t));

    // Gắn servo vào pin
    servo.attach(SERVO_PIN);
    servo.write(0);
    currentAngle = 0;

    // Tạo FreeRTOS task trên Core 1
    xTaskCreatePinnedToCore(
        servoTask,      // hàm task
        "ServoTask",    // tên task (debug)
        2048,           // stack size (bytes)
        nullptr,        // tham số truyền vào task
        1,              // priority
        nullptr,        // task handle (không cần lưu)
        1               // chạy trên Core 1
    );

    Serial.println("[SERVO] Da khoi tao, task dang chay tren Core 1");
}

void setServoAngle(uint8_t angle) {
    if (servoQueue == nullptr) return;
    // Ghi đè lệnh cũ nếu queue đã có phần tử
    xQueueOverwrite(servoQueue, &angle);
}

uint8_t getServoAngle() {
    return currentAngle;
}
