#include "wifi_manager.h"
#include <WiFi.h>
#include <Arduino.h>
#include "config.h"

#include "display.h"

void setupWifi() {
    Serial.print("\n[WIFI] Dang ket noi toi ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\n[WIFI] Da ket noi thanh cong!");
    Serial.print("[WIFI] Dia chi IP: ");
    Serial.println(WiFi.localIP());
}

void keepWifiConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n[WIFI] Mat ket noi! Dang thu ket noi lai...");
        setupWifi();
    }
}