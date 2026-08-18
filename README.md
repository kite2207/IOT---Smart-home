# SafeHome

SafeHome là dự án nhà thông minh sử dụng ESP32 làm bộ điều khiển trung tâm. Hệ thống kết nối MQTT và Node-RED để điều khiển, giám sát đèn, quạt qua relay, servo cửa, đồng thời thu thập dữ liệu từ cảm biến DHT11, cảm biến siêu âm và RFID.

## Chức năng chính

- Điều khiển đèn LED qua MQTT.
- Tự động điều khiển quạt dựa trên nhiệt độ và độ ẩm từ DHT11.
- Điều khiển servo đóng/mở cửa.
- Phát hiện vật thể ở khoảng cách nhỏ hơn 11 cm bằng cảm biến HC-SR04.
- Đọc thẻ RFID MFRC522.
- Hiển thị thông tin trên LCD I2C.
- Giám sát và điều khiển thiết bị trên giao diện Node-RED.

## Phần cứng

- ESP32 DevKit.
- Cảm biến DHT11: DATA → GPIO4.
- HC-SR04: TRIG → GPIO26, ECHO → GPIO27.
- Servo: tín hiệu → GPIO14.
- Relay/quạt: chân IN → GPIO32.
- RFID MFRC522: SS → GPIO5, SCK → GPIO18, MISO → GPIO19, MOSI → GPIO23, RST → GPIO25.
- LCD I2C: SDA → GPIO21, SCL → GPIO22.

Motor và servo nên sử dụng nguồn riêng phù hợp; nối chung GND với ESP32.

## Cài đặt

1. Cài Visual Studio Code và extension PlatformIO.
2. Mở thư mục project bằng VS Code.
3. Kết nối ESP32 qua cáp USB.
4. Kiểm tra cổng COM trong `platformio.ini` và chỉnh `upload_port`, `monitor_port` nếu cần.

## Build và nạp code

Mở Terminal trong VS Code và chạy:

```powershell
pio run -e hardware
pio run -e hardware -t upload
```

Mở Serial Monitor với baudrate 115200:

```powershell
pio device monitor -e hardware
```

Thoát Serial Monitor bằng `Ctrl+C`.

## Chạy mô phỏng Wokwi

Environment `wokwi` đang sử dụng cảm biến DHT22 theo `diagram.json`:

```powershell
pio run -e wokwi
```

Nếu sử dụng Wokwi VS Code Extension, mở `diagram.json` và nhấn nút Start Simulator.

## MQTT topics

| Chức năng | Command | State |
|---|---|---|
| LED | `safehome/led/set` | `safehome/led/state` |
| Quạt/relay | `safehome/fan/set` | `safehome/fan/state` |
| Servo | `safehome/servo/set` | `safehome/servo/state` |
| DHT nhiệt độ | - | `safehome/dht/temperature` |
| DHT độ ẩm | - | `safehome/dht/humidity` |
| Siêu âm | - | `safehome/ultrasonic` |
| LCD | `safehome/lcd/display` | - |

Payload quạt và LED có thể dùng `true`/`false` hoặc `ON`/`OFF`. Servo hỗ trợ `true` để mở, `false` để đóng, hoặc gửi trực tiếp góc từ `0` đến `180`.
