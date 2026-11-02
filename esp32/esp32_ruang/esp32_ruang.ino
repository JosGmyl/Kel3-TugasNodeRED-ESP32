#include "EspMQTTClient.h"
#include <DHT.h>


// =====================================================================
// BAGIAN YANG DIUBAH SESUAI KELOMPOK, RUANGAN, DAN HOTSPOT HP
// =====================================================================
#define KELOMPOK  "klp3"              // Sesuaikan dengan topik di Node-RED (default: klp3)
#define ROOM      "ruang3"            // E7: Ganti "ruang1", "ruang2", atau "ruang3" tiap ganti board
#define WIFI_SSID "SSID_LAB"   // Ganti dengan nama Wi-Fi / Hotspot HP (Wajib 2.4 GHz)
#define WIFI_PASS "PASSWORD_LAB"   // Ganti dengan password Wi-Fi / Hotspot HP
#define BROKER    "broker.mqttdashboard.com"

// =====================================================================
// PEMBENTUKAN TOPIC & CLIENT ID OTOMATIS (Syarat E2 & E7)
// =====================================================================
#define PREFIX    "pens/" KELOMPOK "/" ROOM
const char* T_TEMP   = PREFIX "/temperature";
const char* T_HUM    = PREFIX "/humidity";
const char* T_L1     = PREFIX "/lamp1";
const char* T_L2     = PREFIX "/lamp2";
const char* T_STATUS = PREFIX "/status";

// =====================================================================
// KONFIGURASI PIN HARDWARE (Otomatis untuk ESP32 V1 & ESP32-S3)
// =====================================================================
#define DHTPIN  16
#define DHTTYPE DHT11

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  const int L1_PIN = 4;
  const int L2_PIN = 9;
#else
  const int L1_PIN = 25;
  const int L2_PIN = 26;
#endif

DHT dht(DHTPIN, DHTTYPE);

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASS,
  BROKER,
  "",
  "",
  "esp32-" KELOMPOK "-" ROOM,
  1883
);

void setLamp(int pin, const String& payload) {
  if (payload == "1") {
    digitalWrite(pin, HIGH);
    Serial.printf("[LAMPU] Pin GPIO %d -> NYALA (1)\n", pin);
  } else if (payload == "0") {
    digitalWrite(pin, LOW);
    Serial.printf("[LAMPU] Pin GPIO %d -> MATI (0)\n", pin);
  } else {
    Serial.printf("[ABAIKAN] Payload tidak valid: %s\n", payload.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(L1_PIN, OUTPUT);
  pinMode(L2_PIN, OUTPUT);
  digitalWrite(L1_PIN, LOW);
  digitalWrite(L2_PIN, LOW);
  dht.begin();

  client.enableDebuggingMessages();
  client.setKeepAlive(15);
  client.enableLastWillMessage(T_STATUS, "offline", true);
}

void onConnectionEstablished() {
  Serial.println("=========================================");
  Serial.println("MQTT Tersambung! Mengatur Subscribe & Status...");
  Serial.printf("Ruangan Aktif : %s\n", ROOM);
  Serial.printf("Pin Lampu 1   : GPIO %d\n", L1_PIN);
  Serial.printf("Pin Lampu 2   : GPIO %d\n", L2_PIN);
  Serial.println("=========================================");

  client.subscribe(T_L1, [](const String& payload) {
    setLamp(L1_PIN, payload);
  }, 1);

  client.subscribe(T_L2, [](const String& payload) {
    setLamp(L2_PIN, payload);
  }, 1);

  client.publish(T_STATUS, "online", true);
}

unsigned long lastPublishTime = 0;

void loop() {
  client.loop();

  unsigned long now = millis();
  if (now - lastPublishTime >= 5000) {
    lastPublishTime = now;

    if (client.isConnected()) {
      float temp = dht.readTemperature();
      float hum  = dht.readHumidity();

      if (!isnan(temp)) {
        String tempStr = String(temp, 1);
        client.publish(T_TEMP, tempStr, false);
        Serial.printf("[SENSOR] Publish Suhu       : %s C -> %s\n", tempStr.c_str(), T_TEMP);
      } else {
        Serial.println("[ERROR] Gagal membaca suhu dari DHT11! Cek kabel GPIO 16.");
      }

      if (!isnan(hum)) {
        String humStr = String(hum, 1);
        client.publish(T_HUM, humStr, false);
        Serial.printf("[SENSOR] Publish Kelembapan : %s %% -> %s\n", humStr.c_str(), T_HUM);
      } else {
        Serial.println("[ERROR] Gagal membaca kelembapan dari DHT11! Cek kabel GPIO 16.");
      }
    }
  }
}