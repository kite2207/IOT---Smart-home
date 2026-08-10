// supabase_client.cpp
// Gửi dữ liệu sensor lên Supabase PostgreSQL qua HTTPS REST API.
//
// Endpoint: POST https://<project>.supabase.co/rest/v1/sensor_logs
// Auth:     apikey + Authorization: Bearer <anon_key>
// Body:     JSON { sensor_type, value_num, value_text, unit, device_id }
//
// Lưu ý Wokwi:
//   - Wokwi hỗ trợ HTTPS (port 443) qua WiFiClientSecure với setInsecure().
//   - HTTPClient + WiFiClientSecure dùng stack ~6KB, task DHT cần tăng lên 8192.

#include "supabase_client.h"
#include "config.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

bool supabaseInsert(const char* sensorType,
                    float       valueNum,
                    const char* valueText,
                    const char* unit)
{
    // Kiểm tra WiFi còn kết nối không
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Supabase] Bo qua: WiFi chua ket noi.");
        return false;
    }

    // Build URL endpoint
    char url[128];
    snprintf(url, sizeof(url),
             "https://%s.supabase.co/rest/v1/sensor_logs",
             SUPABASE_PROJECT_ID);

    // Build JSON payload
    // Dùng String để dễ xử lý NAN và nullptr
    String body = "{";
    body += "\"sensor_type\":\"";
    body += sensorType;
    body += "\"";

    if (!isnan(valueNum)) {
        body += ",\"value_num\":";
        body += String(valueNum, 2);   // 2 chữ số thập phân
    } else {
        body += ",\"value_num\":null";
    }

    if (valueText != nullptr) {
        body += ",\"value_text\":\"";
        body += valueText;
        body += "\"";
    } else {
        body += ",\"value_text\":null";
    }

    if (unit != nullptr) {
        body += ",\"unit\":\"";
        body += unit;
        body += "\"";
    } else {
        body += ",\"unit\":null";
    }

    body += ",\"device_id\":\"esp32-safehome\"";
    body += "}";

    // Tạo HTTPS client (setInsecure = bỏ qua verify cert, phù hợp Wokwi)
    WiFiClientSecure tlsClient;
    tlsClient.setInsecure();

    HTTPClient http;
    http.begin(tlsClient, url);

    // Set headers theo Supabase REST API spec
    http.addHeader("apikey",        SUPABASE_ANON_KEY);
    http.addHeader("Authorization", String("Bearer ") + SUPABASE_ANON_KEY);
    http.addHeader("Content-Type",  "application/json");
    http.addHeader("Prefer",        "return=minimal");  // Không trả body → nhẹ hơn
    http.setTimeout(8000);  // 8 giây timeout

    Serial.printf("[Supabase] POST %s | %s\n", sensorType, body.c_str());

    int httpCode = http.POST(body);

    bool success = (httpCode == 201);
    if (success) {
        Serial.printf("[Supabase] OK (HTTP %d) - Da luu: %s\n", httpCode, sensorType);
    } else {
        Serial.printf("[Supabase] LOI HTTP %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
    }

    http.end();
    return success;
}

String supabaseQuery(const char* table, const char* query) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Supabase] Query bo qua: WiFi chua ket noi.");
        return "";
    }

    char url[256];
    snprintf(url, sizeof(url),
             "https://%s.supabase.co/rest/v1/%s?%s",
             SUPABASE_PROJECT_ID, table, query);

    WiFiClientSecure tlsClient;
    tlsClient.setInsecure();

    HTTPClient http;
    http.begin(tlsClient, url);

    http.addHeader("apikey",        SUPABASE_ANON_KEY);
    http.addHeader("Authorization", String("Bearer ") + SUPABASE_ANON_KEY);
    http.addHeader("Content-Type",  "application/json");
    http.setTimeout(8000);

    Serial.printf("[Supabase] GET %s\n", url);

    int httpCode = http.GET();
    String response = "";

    if (httpCode >= 200 && httpCode < 300) {
        response = http.getString();
        Serial.printf("[Supabase] GET OK (HTTP %d)\n", httpCode);
    } else {
        Serial.printf("[Supabase] GET LOI HTTP %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
    }

    http.end();
    return response;
}
