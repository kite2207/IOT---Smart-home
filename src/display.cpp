#include "display.h"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <cstring>

#include "config.h"

namespace {
// Khởi tạo đối tượng LCD nội bộ
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Hàm private: Xử lý logic in và xóa khoảng trắng thừa
void printLine(uint8_t row, const char* text) {
    char line[LCD_COLUMNS + 1] = {};
    strncpy(line, text, LCD_COLUMNS);
    lcd.setCursor(0, row);
    lcd.print(line);

    // Xóa các ký tự cũ còn sót lại trên dòng
    for (uint8_t column = strlen(line); column < LCD_COLUMNS; ++column) {
        lcd.print(' ');
    }
}
}  // namespace

//Hàm PUBLIC

void setupDisplay() {
    lcd.init();         // Khởi tạo giao tiếp I2C với LCD
    lcd.backlight();    // Bật đèn nền
    lcd.clear();        // Làm sạch màn hình lúc khởi động
}

void printDisplayLine(uint8_t row, const char* text) {
    // Chỉ thực hiện in nếu số hàng (row) nằm trong giới hạn của màn hình
    if (row < LCD_ROWS) {
        printLine(row, text);
    }
}

void clearDisplay() {
    lcd.clear();
}