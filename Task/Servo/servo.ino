#include <Arduino.h>
#include <ESP32Servo.h>

#define SERVO1_PIN 4   // Servo pertama (Core 0)
#define SERVO2_PIN 5   // Servo kedua (Core 1)

Servo servo1;
Servo servo2;

void taskServo1(void *pvParameters);
void taskServo2(void *pvParameters);

void setup() {
  Serial.begin(115200);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  xTaskCreatePinnedToCore(taskServo1, "Servo1_Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskServo2, "Servo2_Task", 4096, NULL, 1, NULL, 1);
}

void loop() {}

// Core 0
void taskServo1(void *pvParameters) {
  for (;;) {
    for (int pos = 0; pos <= 180; pos += 5) {
      servo1.write(pos);
      Serial.printf("[Core0] Servo1 → %d°\n", pos);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    for (int pos = 180; pos >= 0; pos -= 5) {
      servo1.write(pos);
      Serial.printf("[Core0] Servo1 → %d°\n", pos);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

// Core 1
void taskServo2(void *pvParameters) {
  for (;;) {
    for (int pos = 0; pos <= 180; pos += 3) {
      servo2.write(pos);
      Serial.printf("[Core1] Servo2 → %d°\n", pos);
      vTaskDelay(60 / portTICK_PERIOD_MS);
    }
    for (int pos = 180; pos >= 0; pos -= 3) {
      servo2.write(pos);
      Serial.printf("[Core1] Servo2 → %d°\n", pos);
      vTaskDelay(60 / portTICK_PERIOD_MS);
    }
  }
}
