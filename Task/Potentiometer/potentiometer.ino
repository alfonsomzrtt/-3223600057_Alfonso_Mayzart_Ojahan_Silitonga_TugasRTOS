#include <Arduino.h>

#define POT1_PIN 1
#define POT2_PIN 2

void taskPot1(void *pvParameters);
void taskPot2(void *pvParameters);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  xTaskCreatePinnedToCore(taskPot1, "Pot1_Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskPot2, "Pot2_Task", 4096, NULL, 1, NULL, 1);
}

void loop() {}

void taskPot1(void *pvParameters) {
  for (;;) {
    int val = analogRead(POT1_PIN);
    Serial.printf("Core0 - Pot1: %d\n", val);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void taskPot2(void *pvParameters) {
  for (;;) {
    int val = analogRead(POT2_PIN);
    Serial.printf("Core1 - Pot2: %d\n", val);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
