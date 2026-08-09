#include "wifi_manager.h"
#include <WiFi.h>
#include <Arduino.h>
#include "config.h"

#include "display.h"

void setupWifi() {
    Serial.print("\n[WIFI] Connecting to ");
    Serial.println(WIFI_SSID);

    printDisplayLine(0, "[WIFI] Connecting");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\n[WIFI] Connected successfully!");
    Serial.print("[WIFI] IP Address: ");
    Serial.println(WiFi.localIP());

    printDisplayLine(0, "[WIFI] Connected!");
}

void keepWifiConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n[WIFI] Connection lost! Reconnecting...");
        printDisplayLine(0, "[WIFI] Lost! Retry");
        setupWifi();
    }
}