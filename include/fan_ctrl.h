#pragma once

// Khởi tạo GPIO motor quạt. Gọi trong setup().
void setupFan();

// Bật (true) hoặc tắt (false) quạt.
void setFan(bool on);

// Trả về trạng thái hiện tại: true = đang bật.
bool fanIsOn();
