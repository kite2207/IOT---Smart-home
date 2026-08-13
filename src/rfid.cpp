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
unsigned long lastDiagnosticTime = 0;
constexpr unsigned long SCAN_COOLDOWN_MS = 1500;
constexpr unsigned long DIAGNOSTIC_INTERVAL_MS = 3000;

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

    if (DEBUG_SERIAL) {
        const bool selfTestPassed = reader.PCD_PerformSelfTest();
        Serial.print("[RFID] Digital self-test: ");
        Serial.println(selfTestPassed ? "PASS" : "FAIL");

        // The self-test resets internal registers, so initialize the reader again.
        reader.PCD_Init();
    }

    reader.PCD_AntennaOn();
    reader.PCD_SetAntennaGain(MFRC522::RxGain_max);

    Serial.println("[RFID] MFRC522 ready");
    if (DEBUG_SERIAL) {
        Serial.print("[RFID] Pins: SS=GPIO");
        Serial.print(RFID_SS_PIN);
        Serial.print(", RST=GPIO");
        Serial.print(RFID_RST_PIN);
        Serial.print(", SCK=GPIO");
        Serial.print(RFID_SCK_PIN);
        Serial.print(", MISO=GPIO");
        Serial.print(RFID_MISO_PIN);
        Serial.print(", MOSI=GPIO");
        Serial.println(RFID_MOSI_PIN);
        reader.PCD_DumpVersionToSerial();
    }
    printDisplayLine(0, "[RFID] Ready");
}

void rfidLoop() {
    const unsigned long now = millis();
    const bool diagnosticDue =
        DEBUG_SERIAL && now - lastDiagnosticTime >= DIAGNOSTIC_INTERVAL_MS;

    if (diagnosticDue) {
        lastDiagnosticTime = now;
        const byte version = reader.PCD_ReadRegister(MFRC522::VersionReg);
        Serial.print("[RFID] Waiting for card; VersionReg=0x");
        if (version < 0x10) {
            Serial.print('0');
        }
        Serial.print(version, HEX);
        if (version == 0x00 || version == 0xFF) {
            Serial.print(" (reader not responding - check SPI wiring and 3.3V)");
        }
        Serial.print(", antenna gain=0x");
        Serial.print(reader.PCD_GetAntennaGain(), HEX);
        Serial.print(", TxControl=0x");
        const byte txControl = reader.PCD_ReadRegister(MFRC522::TxControlReg);
        if (txControl < 0x10) {
            Serial.print('0');
        }
        Serial.print(txControl, HEX);
        Serial.print((txControl & 0x03) == 0x03 ? " (antenna ON)" : " (antenna OFF)");
        Serial.println();
    }

    bool cardPresent = reader.PICC_IsNewCardPresent();
    if (!cardPresent && diagnosticDue) {
        byte atqa[2] = {};
        byte atqaSize = sizeof(atqa);
        const MFRC522::StatusCode status = reader.PICC_WakeupA(atqa, &atqaSize);

        Serial.print("[RFID] RF poll: ");
        Serial.print(MFRC522::GetStatusCodeName(status));
        if (status == MFRC522::STATUS_OK) {
            Serial.print(", ATQA=0x");
            if (atqa[0] < 0x10) {
                Serial.print('0');
            }
            Serial.print(atqa[0], HEX);
            if (atqa[1] < 0x10) {
                Serial.print('0');
            }
            Serial.print(atqa[1], HEX);
        }
        Serial.println();

        cardPresent = status == MFRC522::STATUS_OK ||
                      status == MFRC522::STATUS_COLLISION;
    }

    if (!cardPresent) {
        return;
    }

    if (DEBUG_SERIAL) {
        Serial.println("[RFID] Card detected, reading UID...");
    }

    if (!reader.PICC_ReadCardSerial()) {
        if (DEBUG_SERIAL) {
            Serial.println("[RFID] Card detected but UID read failed");
        }
        return;
    }

    char uid[32] = {};
    printUid(reader.uid.uidByte, reader.uid.size, uid, sizeof(uid));

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

    mqttPublish(RFID_SCAN_TOPIC, uid);
    Serial.println("[RFID] Published scan to MQTT");

    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
}
