const int GPIO_LED0_CORE0 = 2;   
const int GPIO_LED1_CORE1 = 36;   
const int GPIO_LED2_CORE0 = 20;   

TaskHandle_t TaskCore0Handle = NULL;
TaskHandle_t TaskCore1Handle = NULL;


void TaskCore0(void *pvParameters) {
  Serial.printf("TaskCore0 started on core %d\n", xPortGetCoreID());
  
  pinMode(GPIO_LED0_CORE0, OUTPUT);
  pinMode(GPIO_LED2_CORE0, OUTPUT);

  bool led0State = false;
  bool led2State = false;

  for (;;) {
    led0State = !led0State;
    led2State = !led2State;

    digitalWrite(GPIO_LED0_CORE0, led0State ? HIGH : LOW);
    digitalWrite(GPIO_LED2_CORE0, led2State ? HIGH : LOW);

    vTaskDelay(pdMS_TO_TICKS(500)); 
  }
}

void TaskCore1(void *pvParameters) {
  Serial.printf("TaskCore1 started on core %d\n", xPortGetCoreID());
  
  pinMode(GPIO_LED1_CORE1, OUTPUT);
  bool led1State = false;

  for (;;) {
    led1State = !led1State;
    digitalWrite(GPIO_LED1_CORE1, led1State ? HIGH : LOW);
    vTaskDelay(pdMS_TO_TICKS(200)); 
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting tasks...");

  xTaskCreatePinnedToCore(
    TaskCore0,          
    "LED_Core0_Task",   
    2048,              
    NULL,               
    1,                  
    &TaskCore0Handle,   
    0                  
  );

  xTaskCreatePinnedToCore(
    TaskCore1,
    "LED_Core1_Task",
    2048,
    NULL,
    1,
    &TaskCore1Handle,
    1                  
  );

  if (TaskCore0Handle != NULL) Serial.println("TaskCore0 created");
  if (TaskCore1Handle != NULL) Serial.println("TaskCore1 created");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000)); 
}
