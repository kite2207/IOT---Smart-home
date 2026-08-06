// dht_sensor.cpp
// Đọc cảm biến DHT22 (Wokwi) / DHT11 (phần cứng thật) định kỳ,
// tự động điều khiển quạt làm mát khi vượt ngưỡng nhiệt độ / độ ẩm.
//
// Kiến trúc:
//   - dhtTask() chạy trên Core 1 cùng với Arduino loop() và MQTT,
//     tránh race condition với PubSubClient.
//   - Mỗi DHT_READ_INTERVAL_MS đọc sensor một lần.
//   - Nếu temp >= TEMP_THRESHOLD HOẶC humidity >= HUMIDITY_THRESHOLD
//     -> gọi setFan(true) tự động.
//   - Kết quả publish lên MQTT và lưu vào biến shared (mutex-protected).

#include "dht_sensor.h"
#include "config.h"
#include "fan_ctrl.h"
#include "mqtt_manager.h"

#include <Arduino.h>
#include <DHT.h>                    // Adafruit DHT sensor library
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

namespace {
    // Wokwi dùng wokwi-dht22 nên type là DHT22; trên phần cứng thật đổi thành DHT11
    DHT dht(DHT_PIN, DHT22);

    SemaphoreHandle_t readingsMutex = nullptr;
    float latestTemp     = 0.0f;
    float latestHumidity = 0.0f;

    // FreeRTOS Task: Core 1, priority 1
    void dhtTask(void* /*pvParameters*/) {
        // Đợi WiFi + MQTT kết nối xong trước khi bắt đầu đọc
        vTaskDelay(pdMS_TO_TICKS(5000));

        for (;;) {
            float temp = dht.readTemperature();
            float hum  = dht.readHumidity();

            if (!isnan(temp) && !isnan(hum)) {
                // Cập nhật biến shared
                if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                    latestTemp     = temp;
                    latestHumidity = hum;
                    xSemaphoreGive(readingsMutex);
                }

                // Tự động bật/tắt quạt theo ngưỡng
                bool shouldFanOn = (temp >= TEMP_THRESHOLD) || (hum >= HUMIDITY_THRESHOLD);
                setFan(shouldFanOn);

                // Publish lên MQTT
                char buf[12];

                snprintf(buf, sizeof(buf), "%.1f", temp);
                mqttPublish(DHT_TEMPERATURE, buf);

                snprintf(buf, sizeof(buf), "%.1f", hum);
                mqttPublish(DHT_HUMIDITY, buf);

                mqttPublish(FAN_STATE_TOPIC, shouldFanOn ? "ON" : "OFF", true);

                Serial.printf("[DHT] Nhiet do: %.1f C | Do am: %.1f %% | Quat: %s\n",
                              temp, hum, shouldFanOn ? "BAT" : "TAT");
            } else {
                Serial.println("[DHT] Loi doc cam bien! Kiem tra ket noi day.");
            }

            vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
        }
    }
}  // namespace

// --- API public ---

void setupDht() {
    readingsMutex = xSemaphoreCreateMutex();

    dht.begin();  // Adafruit DHT: không cần truyền type vào đây vì đã khai báo lúc khởi tạo

    xTaskCreatePinnedToCore(
        dhtTask,
        "DhtTask",
        3072,    // stack lớn hơn servo vì dùng printf
        nullptr,
        1,
        nullptr,
        1        // Core 1
    );

    Serial.println("[DHT] Da khoi tao, task dang chay tren Core 1");
}

float getDhtTemperature() {
    float t = 0.0f;
    if (readingsMutex && xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        t = latestTemp;
        xSemaphoreGive(readingsMutex);
    }
    return t;
}

float getDhtHumidity() {
    float h = 0.0f;
    if (readingsMutex && xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        h = latestHumidity;
        xSemaphoreGive(readingsMutex);
    }
    return h;
}
