#pragma once

// Khởi tạo DHT11 và tạo FreeRTOS task đọc định kỳ.
// Phải gọi sau setupMqtt() vì task sẽ publish lên MQTT.
void setupDht();

// Lấy nhiệt độ đo được lần cuối (°C). Thread-safe.
float getDhtTemperature();

// Lấy độ ẩm đo được lần cuối (%). Thread-safe.
float getDhtHumidity();
