# -3223600057_Alfonso_Mayzart_Ojahan_Silitonga_TugasRTOS
Tugas RTOS mengakses 8 periferal yang terdiri dari oled, led, buzzer, button, potensiometer, stepper, servo, rotary-encoder di ESP32-S3 pada platform wokwi.com
# Dual-Core ESP32-S3 Multitasking with FreeRTOS  


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


Pembagian Core dan Tugas

| Core       | Periferal                                                     | Tugas                                |
| ---------- | ------------------------------------------------------------- | ------------------------------------ |
| **Core 0** | Rotary1, Pot1, Stepper1, Servo1, LED1, Buzzer1, Button1       | Input dan output pertama             |
| **Core 1** | Rotary2, Pot2, Stepper2, Servo2, LED2, Buzzer2, Button2, OLED | Input/output kedua dan tampilan OLED |

Masing-masing periferal berjalan sendiri, tidak saling berbagi variabel atau interrupt.
Tujuannya agar performa tiap core dapat diamati secara murni tanpa interkoneksi.

Eksekusi
1. Upload salah satu program .ino ke board ESP32-S3.
2. Buka Serial Monitor (baud rate 115200).
3. Amati output task dari kedua core:

[Core0] Servo1 → 90°
[Core1] Servo2 → 45°
[Core0] Stepper1 → CW | Step 4
[Core1] Stepper2 → CCW | Step 8
[Core0] Encoder1 Position: 23
[Core1] Encoder2 Position: -12
[Core1] OLED → Update Display OK

4.Ubah input (putar rotary, tekan tombol, ubah posisi potensiometer).
5. Lihat bagaimana periferal bereaksi real-time tanpa lag.
6. Perhatikan bahwa kedua core berjalan simultan dan task tidak saling mengganggu.

