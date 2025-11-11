# 3223600057_Alfonso_Mayzart_Ojahan_Silitonga_TugasRTOS
Tugas RTOS mengakses 8 periferal yang terdiri dari oled, led, buzzer, button, potensiometer, stepper, servo, rotary-encoder di ESP32-S3 pada platform wokwi.com
# Dual-Core ESP32-S3 Multitasking with FreeRTOS Task 1


Repositori ini berisi serangkaian percobaan untuk **mengeksplorasi kemampuan dua core ESP32-S3** dengan **multitasking berbasis FreeRTOS**.  
Setiap percobaan menjalankan **dua task identik** di **Core 0** dan **Core 1**, menggunakan fungsi **'xTaskCreatePinnedToCore()'**.  

Periferal yang diuji meliputi total 8 jenis perangkat:

---
1.ESP32-S3 Board
2. Potentiometer, Rotary-Encoder KY-040, Stepper Nema-17, Servo SG-90
3. LED, OLED, Buzzer, Button
4. Breadboard dan kabel jumper
5. Arduino IDE/Wokwi.com/PlatformIO

## Tujuan Percobaan
- Memaksimalkan penggunaan **dua core ESP32-S3**.  
- Menerapkan konsep **multitasking** menggunakan **FreeRTOS**.  
- Membagi beban kerja antar core untuk meningkatkan **efisiensi pemrosesan**.  
- Mengamati bagaimana kedua core dapat menjalankan task secara **paralel** dan **stabil**.  
- Menguji komunikasi dan sinkronisasi dasar antar task jika diperlukan.  

---

##  Langkah Pemrograman / Percobaan  

### 1. Persiapan  

- IDE: **Arduino IDE** atau **PlatformIO**  
- Board: `ESP32S3 Dev Module`  
- Library tambahan:  
  - `ESP32Servo.h` (untuk servo)  
  - `Wire.h` + `Adafruit_SSD1306.h` (untuk OLED)  
  - `Arduino.h` (default)  

### 2. Struktur Proyek  

Setiap periferal memiliki sketch terpisah untuk memudahkan eksperimen paralel:  

| No | File | Deskripsi |
|----|------|------------|
| 1 | `servo.ino` | Dua servo dikendalikan di Core 0 & Core 1 |
| 2 | `stepper.ino` | Dua motor bipolar bergerak independen |
| 3 | `potentiometer.ino` | Dua potensiometer dibaca bersamaan |
| 4 | `rotary-encoder.ino` | Dua rotary encoder KY-040 dengan interrupt |
| 5 | `Oled.ino` | OLED menampilkan status task atau nilai sensor |
| 6 | `LED.ino` | Dua LED berkedip dengan ritme berbeda di tiap core |
| 7 | `Buzzer.ino` | satu buzzer menghasilkan frekuensi berbeda di tiap core |
| 8 | `Button.ino` | Dua tombol dibaca di core terpisah untuk interaksi input |

---

## Kegunaan xTaskCreatePinnedToCore()
Fungsi inti FreeRTOS pada ESP32 yang memungkinkan untuk menentukan pada core apa task dijalankan. 

```cpp
xTaskCreatePinnedToCore(
  taskFunction,      // Fungsi task
  "TaskName",        // Nama task
  4096,              // Stack size
  NULL,              // Parameter
  1,                 // Prioritas
  NULL,              // Handle (opsional)
  0                  // Nomor core (0 atau 1)
);
```
Contoh
```cpp
xTaskCreatePinnedToCore(taskServo1, "Servo1_Task", 4096, NULL, 1, NULL, 0);
xTaskCreatePinnedToCore(taskServo2, "Servo2_Task", 4096, NULL, 1, NULL, 1);
```
Artinya:
Task Servo1 dijalankan di Core 0
Task Servo2 dijalankan di Core 1


### 3. Pembagian Core dan Tugas

| Core       | Periferal                                                     | Tugas                                |
| ---------- | ------------------------------------------------------------- | ------------------------------------ |
| **Core 0** | Rotary1, Pot1, Stepper1, Servo1, LED1, Buzzer1, Button1       | Input dan output pertama             |
| **Core 1** | Rotary2, Pot2, Stepper2, Servo2, LED2, Buzzer2, Button2, OLED | Input/output kedua dan tampilan OLED |

Masing-masing periferal berjalan sendiri, tidak saling berbagi variabel atau interrupt.
Tujuannya agar performa tiap core dapat diamati secara murni tanpa interkoneksi.

### Eksekusi Program
1. Upload salah satu program .ino ke board ESP32-S3.
2. Buka Serial Monitor (baud rate 115200).
3. Amati output task dari kedua core:

```
[Core0] Servo1 → 90°
[Core1] Servo2 → 45°
[Core0] Stepper1 → CW | Step 4
[Core1] Stepper2 → CCW | Step 8
[Core0] Encoder1 Position: 23
[Core1] Encoder2 Position: -12
[Core1] OLED → Halo dari CORE 0
```

4. Ubah input (putar rotary, tekan tombol, ubah posisi potensiometer).
5. Lihat bagaimana periferal bereaksi real-time tanpa lag.
6. Perhatikan bahwa kedua core berjalan simultan dan task tidak saling mengganggu.



# ESP32-S3 Dual-Core FreeRTOS Project Task 2

## Deskripsi Proyek
Proyek ini menunjukkan cara memaksimalkan penggunaan **dua core prosesor ESP32-S3** untuk menjalankan beberapa **periferal secara paralel** tanpa saling mengganggu.  
Semua task berjalan di atas **FreeRTOS**, dengan pembagian beban yang efisien antara **Core 0** dan **Core 1**.

Proyek ini dibuat dan diuji menggunakan **Wokwi Simulator**, dengan satu file `main.ino` dan file wiring `diagram.json`.

---

## 🎯 Tujuan
- Memanfaatkan dua core ESP32-S3 untuk menjalankan task secara simultan.  
- Menerapkan konsep multitasking menggunakan FreeRTOS.  
- Membagi beban kerja antar-core agar sistem tetap responsif.  
- Mengamati pembagian prioritas task dan efeknya terhadap real-time response.  
- Menunjukkan independensi antar-periferal (satu periferal tidak mempengaruhi yang lain).

---

## ⚙️ Perangkat yang digunakan
Total ada **10 periferal** yang terhubung ke ESP32-S3:

| No | Periferal | Fungsi | Core | Catatan |
|----|------------|--------|------|----------|
| 1 | Potensiometer | Input analog (ADC) | Core 0 | Mengukur nilai tegangan |
| 2 | LED (3 warna) | Indikator status | Core 1 | Menyala bergantian |
| 3 | Push Button (2x) | Input digital | Core 0 | Deteksi tekanan tombol |
| 4 | Buzzer | Output audio | Core 1 | Beep periodik |
| 5 | Stepper Motor (Bipolar) | Output mekanik | Core 0 | Bergerak berurutan |
| 6 | Rotary Encoder KY-040 | Input digital | Core 1 | Mendeteksi putaran |
| 7 | Servo Motor | Output PWM | Core 0 | Mengayun maju–mundur |
| 8 | OLED Display SSD1306 | Output I²C | Core 1 | Menampilkan status |
| 9 | Serial Monitor | Debug output | — | Menampilkan semua log |
| 10 | FreeRTOS Task | Software | Core 0/1 | Mengatur multitasking |

---

## Pinout dan Wiring
Semua koneksi tercatat dalam `diagram.json`.  
Contoh sebagian pin utama:

| Periferal | GPIO | Keterangan |
|------------|-------|-------------|
| Potensiometer | 14 | ADC Input |
| LED Merah / Hijau / Oranye | 19 / 20 / 21 | Output digital |
| Tombol 1 / 2 | 37 / 38 | Input digital |
| Buzzer | 36 | PWM Output |
| Stepper A+/A-/B+/B- | 15 / 16 / 7 / 6 | Driver step sequence |
| Encoder CLK / DT | 41 / 42 | Interrupt input |
| Servo PWM | 13 | Output PWM |
| OLED SDA / SCL | 5 / 4 | I²C Interface |

---

## Struktur FreeRTOS Task
Setiap periferal dikontrol oleh satu task, yang dijalankan di core dan prioritas berbeda:

| Task | Core | Prioritas | Fungsi |
|------|------|------------|--------|
| `taskPot` | 0 | 2 | Membaca ADC dari potensiometer |
| `taskLED` | 1 | 2 | Menyalakan LED bergantian |
| `taskButton` | 0 | 5 | Mendeteksi input tombol |
| `taskBuzzer` | 1 | 1 | Mengeluarkan bunyi beep periodik |
| `taskStepper` | 0 | 3 | Menjalankan motor stepper |
| `taskEncoder` | 1 | 4 | Membaca rotary encoder |
| `taskServo` | 0 | 3 | Menggerakkan servo bolak-balik |
| `taskOLED` | 1 | 2 | Menampilkan data di OLED |

---

🚀 Langkah Menjalankan Program

1. Buka proyek ini di Wokwi.
2. Pastikan file berikut tersedia:
  main.ino → berisi seluruh kode FreeRTOS dan logika periferal.
  diagram.json → berisi wiring semua komponen.
3. Klik “Run Simulation”.
4. Buka Serial Monitor (Ctrl + Shift + M).
5. Amati setiap periferal bekerja paralel dan log tampil real-time.
6. Tekan tombol, ubah potensiometer, atau putar encoder untuk melihat respon individual.

