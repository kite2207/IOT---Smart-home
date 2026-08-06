#include "rfid.h"

#include <Arduino.h>
#include <cstring>
#include <SPI.h>
#include <MFRC522.h>

#include "config.h"
#include "display.h"
#include "mqtt_manager.h"

namespace {
MFRC522 reader(RFID_SS_PIN, RFID_RST_PIN);
char lastUid[32] = {};
unsigned long lastScanTime = 0;
constexpr unsigned long SCAN_COOLDOWN_MS = 1500;

void printUid(byte* uid, byte uidSize, char* output, size_t outputSize) {
    size_t position = 0;

    for (byte index = 0; index < uidSize && position + 3 < outputSize; ++index) {
        if (index > 0 && position + 1 < outputSize) {
            output[position++] = ':';
        }

        position += snprintf(
            output + position,
            outputSize - position,
            "%02X",
            uid[index]
        );
    }

    output[position] = '\0';
}
}

void setupRfid() {
    SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN);
    reader.PCD_Init();

    Serial.println("[RFID] MFRC522 ready");
    printDisplayLine(0, "[RFID] Ready");
}

void rfidLoop() {
    if (!reader.PICC_IsNewCardPresent()) {
        return;
    }

    if (!reader.PICC_ReadCardSerial()) {
        return;
    }

    char uid[32] = {};
    printUid(reader.uid.uidByte, reader.uid.size, uid, sizeof(uid));

    const unsigned long now = millis();
    const bool isSameScan = strcmp(uid, lastUid) == 0;
    if (isSameScan && now - lastScanTime < SCAN_COOLDOWN_MS) {
        reader.PICC_HaltA();
        reader.PCD_StopCrypto1();
        return;
    }

    strncpy(lastUid, uid, sizeof(lastUid) - 1);
    lastScanTime = now;

    Serial.print("[RFID] UID: ");
    Serial.println(uid);
    printDisplayLine(0, "[RFID] Scanned");
    printDisplayLine(1, uid);

    if (!publishMqttMessage(RFID_SCAN_TOPIC, uid)) {
        Serial.println("[RFID] Failed to publish scan (MQTT disconnected)");
    }

    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
}
