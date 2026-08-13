// dht_sensor.cpp
// Doc cam bien DHT22 (Wokwi) / DHT11 (phan cung that) dinh ky,
// tu dong dieu khien quat lam mat khi vuot nguong nhiet do / do am.
//
// Kien truc:
//   - dhtTask() chay tren Core 1 cung voi Arduino loop() va MQTT,
//     tranh race condition voi PubSubClient.
//   - Moi DHT_READ_INTERVAL_MS doc sensor mot lan.
//   - Neu temp >= TEMP_THRESHOLD HOAC humidity >= HUMIDITY_THRESHOLD
//     -> goi setFan(true) tu dong.
//   - Ket qua publish len MQTT; Node-RED se ghi Supabase (khong qua ESP32).

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
        // Doi WiFi + MQTT ket noi xong truoc khi bat dau doc
        vTaskDelay(pdMS_TO_TICKS(5000));

        for (;;) {
            float temp = dht.readTemperature();
            float hum  = dht.readHumidity();

            if (!isnan(temp) && !isnan(hum)) {
                // Cap nhat bien shared
                if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                    latestTemp     = temp;
                    latestHumidity = hum;
                    xSemaphoreGive(readingsMutex);
                }

                // Tu dong bat/tat quat theo nguong
                bool shouldFanOn = (temp >= TEMP_THRESHOLD) || (hum >= HUMIDITY_THRESHOLD);
                setFan(shouldFanOn);

                // Publish len MQTT (real-time, moi lan doc)
                // Node-RED se lang nghe cac topic nay va ghi Supabase
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

    dht.begin();

    xTaskCreatePinnedToCore(
        dhtTask,
        "DhtTask",
        8192,    // Tang len 8192 vi HTTPS/TLS ngot nhieu RAM
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
