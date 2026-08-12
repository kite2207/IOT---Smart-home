#pragma once
#include <stdint.h>

// Hàm khởi tạo màn hình (Gọi trong setup)
void setupDisplay();

// Hàm hiển thị văn bản ra màn hình tại một dòng cụ thể
bool printDisplayLine(uint8_t row, const char* text);

// Hàm xóa toàn bộ màn hình
void clearDisplay();
