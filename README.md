# Kel3-TugasNodeRED-ESP32
### Smart Multi-Room Environmental Monitoring & Actuator Control System via MQTT and Node-RED

> Proyek integrasi *Internet of Things* (IoT) berbasis mikrokontroler ESP32 dan antarmuka web Node-RED Dashboard untuk pemantauan telemetri multi-ruangan serta pengendalian aktuator dua arah secara *real-time* melalui protokol MQTT.

Proyek ini merupakan implementasi sistem automasi dan pemantauan lingkungan berbasis *Internet of Things* (IoT) untuk empat ruangan terdistribusi (**ruang1** hingga **ruang4**). Setiap ruangan dioperasikan oleh node mikrokontroler **ESP32** mandiri yang terintegrasi dengan sensor suhu dan kelembapan **DHT11** serta dua aktuator lampu LED yang dapat dikontrol secara dua arah (*bi-directional control*).

Seluruh telemetri data lingkungan dan instruksi kendali ditransmisikan secara *real-time* memanfaatkan protokol ringan **MQTT** (*Message Queuing Telemetry Transport*) melalui broker publik **HiveMQ** (`broker.mqttdashboard.com:1883`). Untuk menjamin keamanan jalur komunikasi di broker publik, seluruh perancangan topik dienkapsulasi menggunakan prefix kelompok `pens/klp3/...`. Sistem dipantau dan dikendalikan secara terpusat melalui antarmuka web interaktif berbasis **Node-RED Dashboard**—menyajikan visualisasi tren suhu *multi-series*, pengukur (*gauge*) kelembapan presisi, indikator status konektivitas perangkat (*online/offline*) berbasis *Last Will and Testament* (LWT) dan *retained message*, serta panel kendali 8 saklar lampu secara seketika.

---

## 🎥 Video

| Video | Durasi | Link |
|---|---|---|
| Video presentasi | maks. 7 menit | [YouTube: Video Presentasi](https://youtu.be/euNMfejJ7-Y?feature=shared) |
| Video demo alat | maks. 5 menit | [YouTube: Video Demo Alat](https://youtu.be/euNMfejJ7-Y?feature=shared) |

---

## Supported By

> - Dosen Pengampu : Akhmad Hendriawan ST., MT. (NIP.197501272002121003)
> - Mata Kuliah : IoT dan Jaringan Sensor
> - Program Studi : Sarjana Terapan Teknik Elektronika
> - Politeknik Elektronika Negeri Surabaya

---

## Team Member

| No | NRP | Nama | Ruangan | Jobdesk | GitHub |
|---|---|---|---|---|---|
| 1 | 2124600061 | Agmila Salsabila | `ruang1` | README & dokumentasi | [@agmilaoca-pixel](https://github.com/agmilaoca-pixel) |
| 2 | 2124600073 | Muhammad Ziat Thoriq | `ruang2` | Program ESP32 | [@Zthorrr](https://github.com/Zthorrr) |
| 3 | 2124600076 | Joshua Gamylael | `ruang3` | Flow & dashboard Node-RED | [@JosGmyl](https://github.com/JosGmyl) |
| 4 | 2124600077 | Fadiaz Arta Maulana | `ruang4` | Hardware & pengujian | [@fadiazarts](https://github.com/fadiazarts) |

> Setiap anggota memegang satu ruangan. Jobdesk boleh disesuaikan.

---

## Daftar Isi

- [Deskripsi Project](#kel3-tugasnodered-esp32)
- [Video](#-video)
- [Team Member](#team-member)
- [Arsitektur Sistem](#arsitektur-sistem)
- [Perangkat Keras](#perangkat-keras)
- [Rancangan Topic MQTT](#rancangan-topic-mqtt)
- [Cara Menjalankan](#cara-menjalankan)
- [Screenshot Flow & Dashboard](#screenshot-flow--dashboard)
- [Hasil Pengujian](#hasil-pengujian-t1t12)
- [Ruangan yang Disimulasikan](#ruangan-yang-disimulasikan)
- [Kendala dan Solusi](#kendala-dan-solusi)
- [Pembagian Tugas](#pembagian-tugas)

---

## Arsitektur Sistem

![Blok Diagram](docs/images/blok-diagram.png)

- **ESP32** membaca DHT11 tiap 5 detik → publish `temperature` & `humidity`,
  mendengarkan `lamp1`/`lamp2`, dan mengumumkan status `online`/`offline` (LWT).
- **Broker** meneruskan pesan antar client (QoS 0 untuk sensor, QoS 1 untuk lampu).
- **Node-RED** berlangganan dengan wildcard dan menampilkan dashboard di `/ui`,
  serta mengirim perintah lampu ke `.../ruangN/lamp1` dan `.../ruangN/lamp2`.

---

## Perangkat Keras

### Komponen per ruangan

| Komponen | Jumlah |
|---|---|
| ESP32 (DevKit V1) | 1 |
| Sensor DHT11 | 1 |
| LED (lampu 1 & lampu 2) | 2 |
| Resistor 220 Ω | 2 |
| Kabel jumper & breadboard | secukupnya |

### Tabel pin (saran)

| Komponen | Pin ESP32 | Catatan |
|---|---|---|
| DHT11 – VCC | 3.3V | |
| DHT11 – GND | GND | |
| DHT11 – DATA | GPIO16 | Robo ESP32 Cytron: port Grove 1 |
| LED lampu 1 + resistor 220 Ω | GPIO25 | Anoda ke pin, katoda ke GND lewat resistor |
| LED lampu 2 + resistor 220 Ω | GPIO26 | Sama seperti lampu 1 |

> Pin lain diperbolehkan selama dicatat di laporan.

### Foto rangkaian

![Rangkaian](docs/images/rangkaian.jpeg)

---

## Rancangan Topic MQTT

Prefix wajib: `pens/klp3/<nama_ruang>/<fungsi>`.

| Topic | Arah | Payload | QoS | Retain |
|---|---|---|---|---|
| `pens/klp3/ruangN/temperature` | ESP32 → Node-RED | °C, 1 desimal, mis. `28.5` | 0 | tidak |
| `pens/klp3/ruangN/humidity` | ESP32 → Node-RED | %, 1 desimal, mis. `65.0` | 0 | tidak |
| `pens/klp3/ruangN/lamp1` | Node-RED → ESP32 | `1` = nyala, `0` = mati | 1 | tidak |
| `pens/klp3/ruangN/lamp2` | Node-RED → ESP32 | `1` = nyala, `0` = mati | 1 | tidak |
| `pens/klp3/ruangN/status` | ESP32 / LWT | `online` / `offline` | 0 | ya |

**Wildcard yang dipakai di node `mqtt in` Node-RED:**

| Tujuan | Topic subscribe |
|---|---|
| Suhu semua ruangan | `pens/klp3/+/temperature` |
| Kelembapan semua ruangan | `pens/klp3/+/humidity` |
| Status semua ruangan | `pens/klp3/+/status` |
| Semua topic kelompok (untuk cek) | `pens/klp3/#` |

**Broker:** `broker.mqttdashboard.com:1883` (HiveMQ publik).

---

## Cara Menjalankan

### 1. Kebutuhan Library Arduino

Pastikan Arduino IDE telah terpasang board package ESP32 dan pasang library berikut via *Sketch → Include Library → Manage Libraries*:

- **`EspMQTTClient`** oleh Patrick Lapointe (penanganan koneksi Wi-Fi, MQTT, auto-reconnect, dan LWT)
- **`DHT sensor library`** oleh Adafruit (+ `Adafruit Unified Sensor` bila diminta)

### 2. Konfigurasi dan Upload Firmware ESP32

1. Buka berkas `esp32/esp32_ruang/esp32_ruang.ino` pada Arduino IDE.
2. Sesuaikan konfigurasi jaringan Wi-Fi pada baris kode:
   ```cpp
   #define WIFI_SSID "SSID_PILIHAN_ANDA"   // Nama Wi-Fi / Hotspot HP (Wajib 2.4 GHz)
   #define WIFI_PASS "PASSWORD_ANDA"       // Password Wi-Fi
   ```
   > **Catatan Keamanan:** Hindari mengunggah SSID dan password asli ke repositori publik. Selalu kembalikan nilai ke placeholder `"SSID_LAB"` dan `"PASSWORD_LAB"` sebelum melakukan commit/push ke GitHub.
3. Atur identitas ruangan pada konstanta `ROOM` sesuai modul board yang di-flash:
   ```cpp
   #define ROOM "ruang1" // Ganti menjadi ruang1, ruang2, ruang3, atau ruang4
   ```
   *(Konfigurasi topik MQTT dan Client ID unik akan terbentuk secara otomatis sesuai syarat E2 & E7).*
4. Firmware telah mendukung deteksi target board otomatis:
   - **ESP32 DevKit V1**: Sensor DHT11 (`GPIO 16`), Lampu 1 (`GPIO 25`), Lampu 2 (`GPIO 26`).
   - **ESP32-S3**: Sensor DHT11 (`GPIO 16`), Lampu 1 (`GPIO 4`), Lampu 2 (`GPIO 9`).
5. Sambungkan ESP32 ke komputer, pilih jenis board dan port COM yang sesuai, kemudian lakukan **Upload**.
6. Buka **Serial Monitor** (baud rate `115200`). Pesan `MQTT Connection established.` akan muncul saat perangkat berhasil terhubung ke broker.

### 3. Konfigurasi dan Import Flow Node-RED

1. Pastikan modul palette dashboard telah terpasang di Node-RED:
   - Buka menu *Manage palette* → tab *Install* → cari dan instal `node-red-dashboard`.
2. Lakukan impor aliran kerja (*flow*):
   - Klik menu garis tiga (pojok kanan atas) → *Import* → pilih berkas `node-red/flows.json` → klik **Import**.
3. Pastikan konfigurasi node broker MQTT terhubung ke broker publik HiveMQ:
   - Server: `broker.mqttdashboard.com`
   - Port: `1883`
4. Klik tombol **Deploy** di pojok kanan atas untuk mengaktifkan seluruh aliran data.

### 4. Mengakses Dashboard UI

- **Editor Flow Node-RED**: `http://localhost:1880`
- **Dashboard Antarmuka UI**: `http://localhost:1880/ui`
- **Akses Dashboard dari Perangkat Seluler (Smartphone)**:
  Buka browser pada smartphone yang terhubung dalam satu jaringan Wi-Fi/LAN yang sama dengan laptop server, lalu akses:
  `http://<IP_Local_Laptop>:1880/ui` *(contoh: `http://192.168.1.10:1880/ui`)*

---

## Screenshot Flow & Dashboard

Flow Node-RED:

![Flow Node-RED](docs/images/flow.png)

Dashboard `/ui`:

![Dashboard](docs/images/dashboard.png)

---

## Hasil Pengujian (T1–T12)

| No | Uji | Hasil | Keterangan |
|:---:|:---|:---:|:---|
| T1 | Koneksi | ✓ | Board ESP32 berhasil terhubung ke jaringan Wi-Fi lokal dan broker HiveMQ (`broker.mqttdashboard.com:1883`). Serial Monitor mencetak log `MQTT Connection established.` secara stabil tanpa *bootloop* atau *delay* penghambat. |
| T2 | Data 4 ruang | ✓ | Seluruh payload `temperature` dan `humidity` dari keempat ruangan berhasil terkirim secara berkala setiap ±5 detik. Pesan terbaca utuh pada tab Debug Node-RED melalui subscribe wildcard `pens/klp3/+/temperature` dan `pens/klp3/+/humidity`. |
| T3 | Chart suhu | ✓ | Elemen chart line pada UI dashboard (`/ui`) berhasil memplot 4 garis multi-series berwarna berbeda. Fungsi parser pada Node-RED membedakan garis secara presisi berdasarkan pembacaan `msg.topic` masing-masing ruangan. |
| T4 | Sensor berubah | ✓ | Respon sensor dinamis dan terisolasi. Pemberian rangsangan fisik (hembusan udara hangat) pada sensor DHT11 di ruang2 hanya menaikkan metrik suhu/kelembapan ruang2 secara *real-time*, sedangkan data sensor ruang lainnya tetap stabil. |
| T5 | Lampu tepat sasaran | ✓ | Saklar kendali beroperasi tepat sasaran (spesifik). Penekanan saklar Lampu 2 Ruang 3 pada dashboard via MQTT topic QoS 1 secara instan menyalakan aktuator LED fisik di pin GPIO ruangan tersebut tanpa memicu lampu ruangan lain. |
| T6 | Matikan lampu | ✓ | Pengiriman payload `"0"` dari dashboard berhasil mematikan LED terkait secara seketika, membuktikan kendali dua arah (bi-directional) berjalan sinkron tanpa kendala latensi. |
| T7 | Payload salah | ✓ | Mekanisme validasi payload berjalan baik. Pengujian manual via terminal dengan payload non-standar (misalnya string `"ON"`, angka `2`, atau teks acak) diabaikan oleh ESP32; status pin lampu tidak terganggu dan sistem tetap beroperasi normal. |
| T8 | Status online | ✓ | Indikator status seluruh ruangan pada Node-RED Dashboard menampilkan teks `online` sesaat setelah koneksi MQTT terjalin via mekanisme publikasi *retained message*. |
| T9 | LWT (cabut daya) | ✓ | Fitur *Last Will and Testament* (LWT) berjalan sesuai standar MQTT. Pemutusan catu daya ESP32 ruang4 memicu broker mengirimkan payload LWT `offline` secara otomatis dalam rentang 15–20 detik (1,5× keep-alive). |
| T10 | Retained message | ✓ | Node-RED yang di-deploy ulang (*Full Deploy*) maupun di-refresh dapat langsung memuat status koneksi terakhir (`online`/`offline`) dari broker seketika tanpa harus menunggu board ESP32 mem-publish ulang. |
| T11 | Reconnect | ✓ | Fitur auto-reconnect dari pustaka `EspMQTTClient` teruji tangguh. Ketika koneksi access point/hotspot sempat diputus selama 10 detik lalu diaktifkan kembali, ESP32 otomatis pulih dan melanjutkan streaming telemetri secara mandiri. |
| T12 | Akses dari HP | ✓ | Web UI dashboard dapat diakses responsif melalui browser smartphone dalam satu subnet LAN (`http://<IP_laptop>:1880/ui`). Pengontrolan 8 saklar sakelar lampu serta visualisasi grafik berjalan lancar tanpa kendala rendering. |

> Seluruh pengujian T1–T12 **berhasil (✓)** pada saat demo.

---

## Ruangan yang Disimulasikan

Seluruh empat ruangan (**ruang1**, **ruang2**, **ruang3**, dan **ruang4**) menggunakan **modul ESP32 fisik secara langsung**, bukan hasil simulasi. Setiap board dilengkapi sensor DHT11 dan dua lampu LED yang terpasang mandiri, sehingga seluruh pengujian (T1–T12) dijalankan pada perangkat nyata tanpa ruangan yang disimulasikan. Dengan demikian, ketentuan minimal dua ruangan berbasis ESP32 sungguhan telah terpenuhi sepenuhnya.

---

## Kendala dan Solusi

| Kendala | Solusi |
|---|---|
| Pembacaan sensor DHT11 selalu menghasilkan nilai `nan` | Memperbesar interval pembacaan sensor (DHT11 maksimal 1× per detik, kelompok menggunakan interval aman 5 detik) dan memastikan jalur tegangan VCC (3.3V) serta ground terhubung secara kokoh. |
| Dua board ESP32 saling terputus-sambung secara berkala (*reconnect loop*) | Memastikan nilai konstanta `ROOM` pada firmware board berbeda untuk setiap ruangan, sehingga pembentukan Client ID MQTT (`esp32-<KELOMPOK>-<ROOM>`) bersifat unik pada broker publik. |
| Tampilan antarmuka Node-RED Dashboard sempat kosong saat pertama kali di-deploy | Menyelaraskan struktur topik MQTT pada node `mqtt in` dan `mqtt out` agar sama persis (termasuk huruf besar-kecil) dengan topik firmware ESP32, serta memverifikasi keselarasan alamat server broker publik HiveMQ. |
| Aktuator lampu LED tidak merespons perintah penekanan saklar dashboard | Memeriksa konfigurasi *topic* pada saklar dashboard agar terarah ke pin yang tepat, memastikan format payload berupa *string* `"1"` atau `"0"` tanpa karakter spasi tambahan, serta memvalidasi pengkabelan anoda/katoda LED. |
| Status koneksi ruangan di dashboard sempat tertunda saat pergantian kondisi | Memahami karakteristik protokol MQTT di mana broker menunggu toleransi waktu keep-alive (±1,5 × 15 detik = 22,5 detik) sebelum memicu pengiriman pesan LWT (*Last Will and Testament*) saat catu daya modul terputus tiba-tiba. |

---

## Pembagian Tugas

Setiap anggota minimal memiliki **2 commit** pada repositori ini. Tabel di bawah merangkum **2 kontribusi commit utama** dari masing-masing anggota berdasarkan riwayat pengembangan proyek:

| Anggota | Commit 1 | Commit 2 |
|---|---|---|
| **Agmila Salsabila**<br>([@agmilaoca-pixel](https://github.com/agmilaoca-pixel)) | Mengintegrasikan tautan video presentasi arsitektur dan demo alat pada dokumentasi `README.md` serta `video/LINK.md` | Melengkapi profil dan tautan repositori GitHub seluruh anggota kelompok pada tabel Team Member |
| **Muhammad Ziat Thoriq**<br>([@Zthorrr](https://github.com/Zthorrr)) | Mengunggah dokumen laporan proyek (`laporan.pdf`), firmware ESP32 (`esp32_ruang.ino`), konfigurasi aliran data Node-RED (`flows.json`), dan rekapitulasi awal pengujian | Melakukan elaborasi mendalam serta menyusun deskripsi teknis hasil pengujian fungsional sistem (T1–T12) pada dokumentasi utama |
| **Joshua Gamylael**<br>([@JosGmyl](https://github.com/JosGmyl)) | Melakukan inisialisasi awal repositori proyek kelompok di GitHub | Mengintegrasikan diagram blok arsitektur sistem ke dalam direktori dokumentasi serta menyempurnakan tata letak README |
| **Fadiaz Arta Maulana**<br>([@fadiazarts](https://github.com/fadiazarts)) | Merancang kerangka dasar laporan `README.md`, deskripsi proyek, serta pembagian alokasi ruangan anggota | Menata dan menyelaraskan struktur direktori repositori sesuai standarisasi ketentuan tugas mata kuliah |

---

## Referensi

- Slide *Node-RED dan ESP32* (hal. 19–27)
- [Cytron: Getting Started with ESP32 & Node-RED](https://cytron.io/tutorial/getting-started-esp32-and-nodered)
- [EspMQTTClient](https://github.com/plapointe6/EspMQTTClient)
- [Node-RED docs](https://nodered.org/docs/getting-started)
- [Node-RED Dashboard](https://flows.nodered.org/node/node-red-dashboard)
