#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#include "config.h"

namespace {
MFRC522 reader(RFID_SS_PIN, RFID_RST_PIN);
unsigned long lastStatusTime = 0;

void printHexByte(byte value) {
    if (value < 0x10) {
        Serial.print('0');
    }
    Serial.print(value, HEX);
}
}  // namespace

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("[RFID TEST] Starting isolated RC522 test");
    Serial.print("[RFID TEST] SS=GPIO");
    Serial.print(RFID_SS_PIN);
    Serial.print(", RST=GPIO");
    Serial.print(RFID_RST_PIN);
    Serial.print(", SCK=GPIO");
    Serial.print(RFID_SCK_PIN);
    Serial.print(", MISO=GPIO");
    Serial.print(RFID_MISO_PIN);
    Serial.print(", MOSI=GPIO");
    Serial.println(RFID_MOSI_PIN);

    SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN);
    reader.PCD_Init();

    const bool selfTestPassed = reader.PCD_PerformSelfTest();
    Serial.print("[RFID TEST] Digital self-test: ");
    Serial.println(selfTestPassed ? "PASS" : "FAIL");

    reader.PCD_Init();
    reader.PCD_AntennaOn();
    reader.PCD_SetAntennaGain(MFRC522::RxGain_max);
    Serial.println("[RFID TEST] Hold a 13.56 MHz card directly over the antenna");
}

void loop() {
    const unsigned long now = millis();
    if (now - lastStatusTime >= 1000) {
        lastStatusTime = now;

        const byte version = reader.PCD_ReadRegister(MFRC522::VersionReg);
        const byte txControl = reader.PCD_ReadRegister(MFRC522::TxControlReg);
        Serial.print("[RFID TEST] Version=0x");
        printHexByte(version);
        Serial.print(", gain=0x");
        printHexByte(reader.PCD_GetAntennaGain());
        Serial.print(", TxControl=0x");
        printHexByte(txControl);
        Serial.println((txControl & 0x03) == 0x03 ? " (antenna ON)" : " (antenna OFF/RESET)");
    }

    if (!reader.PICC_IsNewCardPresent()) {
        delay(20);
        return;
    }

    Serial.println("[RFID TEST] Card RF response detected");
    if (!reader.PICC_ReadCardSerial()) {
        Serial.println("[RFID TEST] UID read failed");
        delay(100);
        return;
    }

    Serial.print("[RFID TEST] UID:");
    for (byte index = 0; index < reader.uid.size; ++index) {
        Serial.print(' ');
        printHexByte(reader.uid.uidByte[index]);
    }
    Serial.println();

    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
    delay(500);
}
