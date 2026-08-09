# 🗄️ Hướng dẫn tích hợp Supabase — SafeHome IoT

## Kiến trúc luồng dữ liệu

```
ESP32 (Wokwi / phần cứng thật)
   │
   ├──► MQTT Publish (HiveMQ)  ──► Node-RED Dashboard  (real-time hiển thị)
   │
   └──► HTTPS POST (Supabase REST API)  ──► PostgreSQL  (lịch sử dài hạn)
```

**Không cần Node-RED để lưu database!** ESP32 gọi thẳng Supabase REST API mỗi 30 giây.

---

## Bước 1: Tạo project Supabase

1. Truy cập [https://supabase.com](https://supabase.com) → **Start your project**
2. Đăng nhập bằng GitHub / Google
3. **New Project** → đặt tên (ví dụ: `safehome-iot`) → chọn region Singapore
4. Đặt **Database Password** (lưu lại, không cần dùng thường xuyên)
5. Chờ ~2 phút để project khởi tạo xong

---

## Bước 2: Tạo bảng database

1. Vào **SQL Editor** (menu trái)
2. **New Query** → Copy toàn bộ nội dung file [`supabase_schema.sql`](./supabase_schema.sql) → Paste → **Run**
3. Kết quả hiển thị: `sensor_logs table | Created OK` → thành công ✅

### Schema bảng `sensor_logs`:

| Cột | Kiểu | Dữ liệu mẫu |
|-----|------|------------|
| `id` | BIGSERIAL | 1, 2, 3... |
| `recorded_at` | TIMESTAMPTZ | 2026-08-08T09:30:00Z |
| `sensor_type` | TEXT | `temperature`, `humidity`, `ultrasonic`, `fan` |
| `value_num` | NUMERIC | 28.5, 65.0 |
| `value_text` | TEXT | `"Có người"`, `"ON"` |
| `unit` | TEXT | `°C`, `%` |
| `device_id` | TEXT | `esp32-safehome` |

---

## Bước 3: Lấy API credentials

1. Vào **Project Settings** (icon ⚙️ dưới menu trái) → **API**
2. Copy 2 giá trị:

| Giá trị | Ví dụ |
|---------|-------|
| **Project URL** | `https://abcdefghijkl.supabase.co` |
| **anon public key** | `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...` |

> ⚠️ Dùng key `anon` (không phải `service_role`). anon key an toàn hơn để nhúng vào firmware.

---

## Bước 4: Điền credentials vào firmware

Mở file [`include/config.h`](./include/config.h) và sửa 2 dòng cuối:

```cpp
// TRƯỚC (placeholder)
const char* const SUPABASE_PROJECT_ID = "YOUR_PROJECT_ID";
const char* const SUPABASE_ANON_KEY   = "YOUR_SUPABASE_ANON_KEY_HERE";

// SAU (thay bằng thông tin thật của bạn)
const char* const SUPABASE_PROJECT_ID = "abcdefghijkl";           // chỉ phần project ID
const char* const SUPABASE_ANON_KEY   = "eyJhbGciOiJIUzI1NiI..."; // full anon key
```

> 💡 `SUPABASE_PROJECT_ID` chỉ là phần ID (không phải full URL).  
> Ví dụ: từ `https://abcdefghijkl.supabase.co` → lấy `abcdefghijkl`

---

## Bước 5: Build & Flash

```bash
# Wokwi (giả lập)
pio run -e wokwi

# Phần cứng thật
pio run -e hardware --target upload
```

---

## Chu kỳ ghi dữ liệu

| Sensor | MQTT | Supabase DB |
|--------|------|------------|
| DHT22 (nhiệt độ + độ ẩm) | Mỗi **2 giây** | Mỗi **30 giây** |
| Fan state | Mỗi **2 giây** | Mỗi **30 giây** (cùng batch DHT) |
| Ultrasonic | Mỗi **1 giây** | Mỗi **30 giây** |

MQTT vẫn real-time cho dashboard. Supabase ghi chậm hơn để tránh rate limit free tier (~500MB/tháng).

Để thay đổi chu kỳ, sửa trong `config.h`:
```cpp
constexpr uint32_t SUPABASE_DB_INTERVAL_MS = 30000; // 30 giây, đổi tùy ý
```

---

## Kiểm tra hoạt động

### Serial Monitor (PlatformIO)
Sau khi ESP32 kết nối WiFi + MQTT, quan sát log:
```
[Supabase] POST temperature | {"sensor_type":"temperature","value_num":28.50,"value_text":null,"unit":"°C","device_id":"esp32-safehome"}
[Supabase] OK (HTTP 201) - Da luu: temperature
[Supabase] OK (HTTP 201) - Da luu: humidity
[Supabase] OK (HTTP 201) - Da luu: fan
[Supabase] OK (HTTP 201) - Da luu: ultrasonic
```

### Supabase Table Editor
Vào **Table Editor** → chọn `sensor_logs` → quan sát rows mới xuất hiện.

### SQL queries hữu ích
```sql
-- 10 bản ghi mới nhất
SELECT * FROM sensor_logs ORDER BY recorded_at DESC LIMIT 10;

-- Giá trị mới nhất của mỗi sensor
SELECT * FROM latest_sensor_readings;

-- Thống kê trung bình theo giờ (24h gần nhất)
SELECT * FROM hourly_sensor_stats;

-- Nhiệt độ trong 1 giờ qua
SELECT recorded_at, value_num AS temp_c
FROM sensor_logs
WHERE sensor_type = 'temperature'
  AND recorded_at >= NOW() - INTERVAL '1 hour'
ORDER BY recorded_at DESC;
```

---

## Troubleshooting

| Triệu chứng | Nguyên nhân | Giải pháp |
|------------|------------|----------|
| `LOI HTTP 401` | API Key sai | Kiểm tra `SUPABASE_ANON_KEY` trong `config.h` |
| `LOI HTTP 403` | RLS chặn | Chạy lại SQL `CREATE POLICY` trong `supabase_schema.sql` |
| `LOI HTTP -1` | WiFi chưa kết nối | ESP32 tự bỏ qua, thử lại lần tiếp theo |
| Không thấy log Supabase | Chưa đến 30 giây | Đợi `SUPABASE_DB_INTERVAL_MS` ms sau lần khởi động |
| Supabase timeout | Project đang ngủ | Free tier sleep sau 7 ngày, gửi 1 request bất kỳ để wake up |

---

## Cấu trúc files đã thêm/sửa

```
IOT---Smart-home/
├── include/
│   ├── config.h              ← [SỬA] Thêm SUPABASE_PROJECT_ID, SUPABASE_ANON_KEY
│   └── supabase_client.h     ← [MỚI] Header: hàm supabaseInsert()
├── src/
│   ├── supabase_client.cpp   ← [MỚI] HTTPS client gọi Supabase REST API
│   ├── dht_sensor.cpp        ← [SỬA] Gọi supabaseInsert() mỗi 30s, stack 8192
│   └── main.cpp              ← [SỬA] Gọi supabaseInsert() cho ultrasonic
└── supabase_schema.sql       ← [MỚI] SQL tạo bảng, indexes, RLS, views
```
