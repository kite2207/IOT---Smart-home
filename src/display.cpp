#include "display.h"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <cstring>

#include "config.h"

namespace {
LiquidCrystal_I2C* lcd = nullptr;
bool lcdAvailable = false;

bool isCommonLcdAddress(uint8_t address) {
    return (address >= 0x20 && address <= 0x27) ||
           (address >= 0x38 && address <= 0x3F);
}

void printLine(uint8_t row, const char* text) {
    char line[LCD_COLUMNS + 1] = {};
    strncpy(line, text, LCD_COLUMNS);
    lcd->setCursor(0, row);
    lcd->print(line);

    for (uint8_t column = strlen(line); column < LCD_COLUMNS; ++column) {
        lcd->print(' ');
    }
}
}  // namespace

void setupDisplay() {
    Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
    Wire.setClock(100000);

    Serial.print("[I2C] Scanning SDA=GPIO");
    Serial.print(LCD_SDA_PIN);
    Serial.print(", SCL=GPIO");
    Serial.println(LCD_SCL_PIN);

    Serial.print("[I2C] Idle levels: SDA=");
    Serial.print(digitalRead(LCD_SDA_PIN) == HIGH ? "HIGH" : "LOW");
    Serial.print(", SCL=");
    Serial.println(digitalRead(LCD_SCL_PIN) == HIGH ? "HIGH" : "LOW");

    uint8_t detectedLcdAddress = 0;

    for (uint8_t address = 1; address < 127; ++address) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("[I2C] Device found at 0x");
            if (address < 0x10) {
                Serial.print('0');
            }
            Serial.println(address, HEX);

            if (address == LCD_I2C_ADDRESS ||
                (detectedLcdAddress == 0 && isCommonLcdAddress(address))) {
                detectedLcdAddress = address;
            }
        }
    }

    if (detectedLcdAddress == 0) {
        Serial.println("[LCD] No I2C backpack detected");
        return;
    }

    Serial.print("[LCD] Using I2C address 0x");
    Serial.println(detectedLcdAddress, HEX);

    lcd = new LiquidCrystal_I2C(detectedLcdAddress, LCD_COLUMNS, LCD_ROWS);
    lcd->init();
    lcd->backlight();
    lcd->clear();
    lcdAvailable = true;
    Serial.println("[LCD] Initialized successfully");
}

bool printDisplayLine(uint8_t row, const char* text) {
    if (!lcdAvailable || row >= LCD_ROWS) {
        return false;
    }

    printLine(row, text);
    return true;
}

void clearDisplay() {
    if (lcdAvailable) {
        lcd->clear();
    }
}
