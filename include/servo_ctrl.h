#pragma once

#include <stdint.h>

/**
 * @brief Khởi tạo servo và tạo FreeRTOS task riêng để điều khiển.
 *        Phải gọi trong setup().
 */
void setupServo();

/**
 * @brief Đặt góc servo (0–180 độ).
 *        An toàn để gọi từ bất kỳ task nào (thread-safe qua queue).
 * @param angle Góc mong muốn [0, 180]
 */
void setServoAngle(uint8_t angle);

/**
 * @brief Lấy góc hiện tại của servo.
 */
uint8_t getServoAngle();
