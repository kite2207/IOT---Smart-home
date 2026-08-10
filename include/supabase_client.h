#pragma once

// supabase_client.h
// Module gọi Supabase REST API qua HTTPS để lưu dữ liệu sensor.
//
// Kiến trúc:
//   - supabaseInsert() tạo kết nối HTTPS mới (WiFiClientSecure) mỗi lần gọi.
//   - Không dùng chung WiFiClientSecure với MQTT để tránh conflict.
//   - Chạy trên cùng FreeRTOS task với caller (dhtTask / loop), không tạo task riêng.
//
// Cách dùng:
//   supabaseInsert("temperature", 28.5, nullptr, "°C");
//   supabaseInsert("ultrasonic",  NAN,  "Có người", nullptr);

#include <Arduino.h>

/**
 * @brief Gửi 1 bản ghi sensor lên bảng `sensor_logs` trên Supabase.
 *
 * @param sensorType  Loại sensor: "temperature", "humidity", "ultrasonic", "fan"
 * @param valueNum    Giá trị số (NAN nếu không có)
 * @param valueText   Giá trị chuỗi (nullptr nếu không có)
 * @param unit        Đơn vị (nullptr nếu không có)
 * @return true  Nếu server trả về HTTP 201 Created
 * @return false Nếu kết nối thất bại hoặc lỗi HTTP khác
 */
bool supabaseInsert(const char* sensorType,
                    float       valueNum,
                    const char* valueText,
                    const char* unit);

/**
 * @brief Truy vấn dữ liệu từ Supabase REST API (GET).
 *
 * @param table Tên bảng (ví dụ: "sensor_logs")
 * @param query Chuỗi query param (ví dụ: "sensor_type=eq.fan&order=created_at.desc&limit=1")
 * @return String Kết quả JSON trả về từ server, hoặc rỗng nếu lỗi.
 */
String supabaseQuery(const char* table, const char* query);
