#include <Arduino.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === PIN DEFINITIONS ===
#define POT_PIN       14
#define LED_RED       19
#define LED_GREEN     20
#define LED_ORANGE    21
#define BTN1          37
#define BTN2          38
#define BUZZER_PIN    36
#define STEP_A_PLUS   15
#define STEP_A_MINUS  16
#define STEP_B_PLUS   7
#define STEP_B_MINUS  6
#define ENCODER_CLK   41
#define ENCODER_DT    42
#define SERVO_PIN     13
#define OLED_SDA      5
#define OLED_SCL      4

// === GLOBALS ===
Servo servo;
volatile int encoderPos = 0;
int lastEncoderA = 0;
bool dirCW = true;

// === TASK DECLARATIONS ===
void taskPot(void *pvParameters);
void taskLED(void *pvParameters);
void taskButton(void *pvParameters);
void taskBuzzer(void *pvParameters);
void taskStepper(void *pvParameters);
void taskEncoder(void *pvParameters);
void taskServo(void *pvParameters);
void taskOLED(void *pvParameters);

// === ENCODER ISR ===
void IRAM_ATTR readEncoder() {
  int a = digitalRead(ENCODER_CLK);
  int b = digitalRead(ENCODER_DT);
  if (a != lastEncoderA) {
    encoderPos += (a == b) ? 1 : -1;
    lastEncoderA = a;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Dual-Core ESP32-S3 FreeRTOS Demo ===");

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STEP_A_PLUS, OUTPUT);
  pinMode(STEP_A_MINUS, OUTPUT);
  pinMode(STEP_B_PLUS, OUTPUT);
  pinMode(STEP_B_MINUS, OUTPUT);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);

  servo.attach(SERVO_PIN);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("ESP32-S3 Ready");
    display.display();
  }

  // === TASK CREATION ===
  xTaskCreatePinnedToCore(taskPot,     "PotTask",     4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskLED,     "LEDTask",     4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskButton,  "ButtonTask",  4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskBuzzer,  "BuzzTask",    4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskStepper, "StepperTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskEncoder, "EncoderTask", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskServo,   "ServoTask",   4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskOLED,    "OLEDTask",    4096, NULL, 1, NULL, 1);
}

void loop() {} // Semua berjalan di task

// === TASK IMPLEMENTATIONS ===

// 1️⃣ Potentiometer reader
void taskPot(void *pvParameters) {
  while (true) {
    int potValue = analogRead(POT_PIN);
    Serial.printf("[Core%d] POT → %d\n", xPortGetCoreID(), potValue);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// 2️⃣ LED Blinker
void taskLED(void *pvParameters) {
  while (true) {
    digitalWrite(LED_RED, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_ORANGE, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED_ORANGE, LOW);
  }
}

// 3️⃣ Button monitor
void taskButton(void *pvParameters) {
  while (true) {
    bool b1 = !digitalRead(BTN1);
    bool b2 = !digitalRead(BTN2);
    if (b1 || b2) {
      Serial.printf("[Core%d] Button Pressed: %s%s\n",
                    xPortGetCoreID(),
                    b1 ? "BTN1 " : "",
                    b2 ? "BTN2" : "");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// 4️⃣ Buzzer task
void taskBuzzer(void *pvParameters) {
  while (true) {
    tone(BUZZER_PIN, 1000);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    noTone(BUZZER_PIN);
    vTaskDelay(700 / portTICK_PERIOD_MS);
  }
}

// 5️⃣ Stepper motor (bipolar 4-step)
void taskStepper(void *pvParameters) {
  const int seq[4][4] = {
    {1, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 1},
    {1, 0, 0, 1}
  };
  int step = 0;
  while (true) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(STEP_A_PLUS, seq[i][0]);
      digitalWrite(STEP_A_MINUS, seq[i][1]);
      digitalWrite(STEP_B_PLUS, seq[i][2]);
      digitalWrite(STEP_B_MINUS, seq[i][3]);
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    if (++step % 50 == 0)
      Serial.printf("[Core%d] Stepper running: step=%d\n", xPortGetCoreID(), step);
  }
}

// 6️⃣ Rotary Encoder
void taskEncoder(void *pvParameters) {
  int last = 0;
  while (true) {
    if (encoderPos != last) {
      Serial.printf("[Core%d] Encoder → %d\n", xPortGetCoreID(), encoderPos);
      last = encoderPos;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// 7️⃣ Servo motor
void taskServo(void *pvParameters) {
  while (true) {
    for (int pos = 0; pos <= 180; pos += 10) {
      servo.write(pos);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    for (int pos = 180; pos >= 0; pos -= 10) {
      servo.write(pos);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

// 8️⃣ OLED Display
void taskOLED(void *pvParameters) {
  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.printf("Core%d Active\n", xPortGetCoreID());
    display.printf("Encoder: %d\n", encoderPos);
    display.display();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
