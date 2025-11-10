#include <Arduino.h>
#include <AccelStepper.h>

// Stepper 1 (Core 0)
#define A1_1  15
#define A2_1  16
#define B1_1  17
#define B2_1  18

// Stepper 2 (Core 1)
#define A1_2  19
#define A2_2  20
#define B1_2  21
#define B2_2  10   // Ganti 22 karena tidak ada di ESP32-S3

void taskStepper1(void *pvParameters);
void taskStepper2(void *pvParameters);

// Urutan 4 langkah standar untuk bipolar motor
const int seq[4][4] = {
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
  Serial.begin(115200);
  delay(500); // Tunggu koneksi serial siap
  Serial.println("Dual Core Bipolar Stepper Test");

  // Stepper 1 pins
  pinMode(A1_1, OUTPUT);
  pinMode(A2_1, OUTPUT);
  pinMode(B1_1, OUTPUT);
  pinMode(B2_1, OUTPUT);

  // Stepper 2 pins
  pinMode(A1_2, OUTPUT);
  pinMode(A2_2, OUTPUT);
  pinMode(B1_2, OUTPUT);
  pinMode(B2_2, OUTPUT);

  // Buat task di core masing-masing
  xTaskCreatePinnedToCore(taskStepper1, "Stepper1_Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskStepper2, "Stepper2_Task", 4096, NULL, 1, NULL, 1);
}

void loop() {}

// Langkah motor untuk satu arah
void stepMotor(int a1, int a2, int b1, int b2, bool cw, int stepDelay) {
  for (int s = 0; s < 4; s++) {
    int idx = cw ? s : (3 - s);
    digitalWrite(a1, seq[idx][0]);
    digitalWrite(a2, seq[idx][1]);
    digitalWrite(b1, seq[idx][2]);
    digitalWrite(b2, seq[idx][3]);
    delayMicroseconds(stepDelay);
  }
}

// Core 0 – Stepper 1
void taskStepper1(void *pvParameters) {
  int stepCount = 0;
  while (true) {
    Serial.printf("[Core0] Stepper1 → CW | Step %d\n", stepCount++);
    stepMotor(A1_1, A2_1, B1_1, B2_1, true, 1200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    Serial.printf("[Core0] Stepper1 → CCW | Step %d\n", stepCount++);
    stepMotor(A1_1, A2_1, B1_1, B2_1, false, 1200);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Core 1 – Stepper 2
void taskStepper2(void *pvParameters) {
  int stepCount = 0;
  while (true) {
    Serial.printf("[Core1] Stepper2 → CW | Step %d\n", stepCount++);
    stepMotor(A1_2, A2_2, B1_2, B2_2, true, 1000);
    vTaskDelay(80 / portTICK_PERIOD_MS);

    Serial.printf("[Core1] Stepper2 → CCW | Step %d\n", stepCount++);
    stepMotor(A1_2, A2_2, B1_2, B2_2, false, 1000);
    vTaskDelay(80 / portTICK_PERIOD_MS);
  }
}
