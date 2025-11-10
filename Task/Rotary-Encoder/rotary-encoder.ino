#include <Arduino.h>

// Encoder 1 (Core 0)
#define ENC1_A 16   // CLK1
#define ENC1_B 17   // DT1

// Encoder 2 (Core 1)
#define ENC2_A 18   // CLK2
#define ENC2_B 19   // DT2

// Variabel posisi
volatile long encoder1Pos = 0;
volatile long encoder2Pos = 0;

// ISR (Interrupt Service Routine) untuk masing-masing encoder
void IRAM_ATTR readEncoder1() {
  int b = digitalRead(ENC1_B);
  encoder1Pos += (b == HIGH) ? 1 : -1;
}

void IRAM_ATTR readEncoder2() {
  int b = digitalRead(ENC2_B);
  encoder2Pos += (b == HIGH) ? 1 : -1;
}

// Task untuk menampilkan data di core masing-masing
void taskEncoder1(void *pvParameters);
void taskEncoder2(void *pvParameters);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Dual Core KY-040 Rotary Encoder Test");

  // Konfigurasi pin
  pinMode(ENC1_A, INPUT_PULLUP);
  pinMode(ENC1_B, INPUT_PULLUP);
  pinMode(ENC2_A, INPUT_PULLUP);
  pinMode(ENC2_B, INPUT_PULLUP);

  // Attach interrupt
  attachInterrupt(digitalPinToInterrupt(ENC1_A), readEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC2_A), readEncoder2, CHANGE);

  // Buat task di core masing-masing
  xTaskCreatePinnedToCore(taskEncoder1, "Encoder1_Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskEncoder2, "Encoder2_Task", 4096, NULL, 1, NULL, 1);
}

void loop() {}

// Core 0 – Encoder 1
void taskEncoder1(void *pvParameters) {
  long lastPos = 0;
  for (;;) {
    if (encoder1Pos != lastPos) {
      Serial.printf("[Core0] Encoder1 Position: %ld\n", encoder1Pos);
      lastPos = encoder1Pos;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Core 1 – Encoder 2
void taskEncoder2(void *pvParameters) {
  long lastPos = 0;
  for (;;) {
    if (encoder2Pos != lastPos) {
      Serial.printf("[Core1] Encoder2 Position: %ld\n", encoder2Pos);
      lastPos = encoder2Pos;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
