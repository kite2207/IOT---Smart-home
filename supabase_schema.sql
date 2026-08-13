-- =====================================================
-- SafeHome IoT - Supabase Database Schema
-- Da tao trong Supabase project: wfkesifcfgajgymipqcd
-- =====================================================

-- Bang rfid_cards: danh sach the RFID duoc phep vao
CREATE TABLE IF NOT EXISTS rfid_cards (
  uid        TEXT PRIMARY KEY,
  owner_name TEXT NOT NULL,
  is_active  BOOLEAN DEFAULT true,
  created_at TIMESTAMPTZ DEFAULT now()
);

-- Bang access_log: lich su quet the RFID
CREATE TABLE IF NOT EXISTS access_log (
  id         BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  uid        TEXT NOT NULL,
  owner_name TEXT,
  result     TEXT NOT NULL,       -- 'GRANTED' / 'DENIED'
  is_anomaly BOOLEAN DEFAULT false,
  scanned_at TIMESTAMPTZ DEFAULT now()
);
CREATE INDEX IF NOT EXISTS idx_access_log_time ON access_log (scanned_at);

-- Bang environment_log: nhiet do / do am / trang thai quat
CREATE TABLE IF NOT EXISTS environment_log (
  id          BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  temperature NUMERIC,
  humidity    NUMERIC,
  fan_state   BOOLEAN,
  recorded_at TIMESTAMPTZ DEFAULT now()
);
CREATE INDEX IF NOT EXISTS idx_env_log_time ON environment_log (recorded_at);

-- Bang profiles: tai khoan nguoi dung (lien ket voi Supabase Auth)
CREATE TABLE IF NOT EXISTS profiles (
  id           UUID PRIMARY KEY REFERENCES auth.users(id),
  display_name TEXT,
  email        TEXT,
  role         TEXT DEFAULT 'member'   -- 'owner' / 'member'
);

-- Bang notifications: lich su thong bao da gui
CREATE TABLE IF NOT EXISTS notifications (
  id         BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  event_type TEXT,
  message    TEXT,
  sent_at    TIMESTAMPTZ DEFAULT now()
);

-- =====================================================
-- Row Level Security (RLS) - cho Node-RED dung anon key
-- =====================================================

-- rfid_cards
ALTER TABLE rfid_cards ENABLE ROW LEVEL SECURITY;
CREATE POLICY IF NOT EXISTS "rfid_cards_read"   ON rfid_cards FOR SELECT USING (true);
CREATE POLICY IF NOT EXISTS "rfid_cards_insert" ON rfid_cards FOR INSERT WITH CHECK (true);

-- access_log
ALTER TABLE access_log ENABLE ROW LEVEL SECURITY;
CREATE POLICY IF NOT EXISTS "access_log_read"   ON access_log FOR SELECT USING (true);
CREATE POLICY IF NOT EXISTS "access_log_insert" ON access_log FOR INSERT WITH CHECK (true);
CREATE POLICY IF NOT EXISTS "access_log_update" ON access_log FOR UPDATE USING (true);

-- environment_log
ALTER TABLE environment_log ENABLE ROW LEVEL SECURITY;
CREATE POLICY IF NOT EXISTS "env_log_read"   ON environment_log FOR SELECT USING (true);
CREATE POLICY IF NOT EXISTS "env_log_insert" ON environment_log FOR INSERT WITH CHECK (true);

-- notifications
ALTER TABLE notifications ENABLE ROW LEVEL SECURITY;
CREATE POLICY IF NOT EXISTS "notif_read"   ON notifications FOR SELECT USING (true);
CREATE POLICY IF NOT EXISTS "notif_insert" ON notifications FOR INSERT WITH CHECK (true);

-- =====================================================
-- Du lieu mau: them the RFID mac dinh de test
-- Sua uid thanh UID the that cua ban
-- =====================================================
INSERT INTO rfid_cards (uid, owner_name, is_active)
VALUES
  ('AA:BB:CC:DD', 'Chu nha',   true),
  ('11:22:33:44', 'Thanh vien', true)
ON CONFLICT (uid) DO NOTHING;
