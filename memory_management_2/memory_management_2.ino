// Use core 0 or 1 based on configuration
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
TaskHandle_t toggleTaskHandle;

volatile bool isMessage = false;
volatile uint32_t bufferloc = 0xFFFFFFFFu;
volatile size_t bufferSize = 0;
SemaphoreHandle_t bufferMutex;

void displayBufferTask(void * param) {
  while (true) {
    if (isMessage) {
      if (bufferloc != 0xFFFFFFFFu) {
        xSemaphoreTake(bufferMutex, portMAX_DELAY); // Lock the buffer
        char *bufhandle = (char *)bufferloc;
        for (size_t i = 0; i < bufferSize; i++) {
          Serial.print(bufhandle[i]);
        }
        Serial.println();
        Serial.println((uint32_t)bufferloc); // Debugging: Print the buffer address
        vPortFree(bufhandle); // Free memory
        bufferloc = 0xFFFFFFFFu;
        bufferSize = 0;
        isMessage = false;
        xSemaphoreGive(bufferMutex); // Unlock the buffer
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to prevent task hogging
  }
}

void readBufferTask(void * param) {
  while (true) {
    if (Serial.available() > 0) {
      char *b = (char *)pvPortMalloc(100 * sizeof(char));
      if (b != NULL) {
        size_t bytesRead = Serial.readBytesUntil('\n', b, 100);
        xSemaphoreTake(bufferMutex, portMAX_DELAY); // Lock the buffer
        bufferloc = (uint32_t)b;
        bufferSize = bytesRead;
        isMessage = true;
        xSemaphoreGive(bufferMutex); // Unlock the buffer
      } else {
        Serial.println("Memory allocation failed!");
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to prevent task hogging
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for initialization

  bufferMutex = xSemaphoreCreateMutex(); // Create a mutex for buffer access
  if (bufferMutex == NULL) {
    Serial.println("Failed to create buffer mutex!");
    while (true);
  }

  xTaskCreatePinnedToCore(
    displayBufferTask,
    "displayBuffer",
    2048, // Stack size
    NULL, // No parameters
    1,    // Task priority
    &toggleTaskHandle, // Task handle
    app_cpu);          // Run on selected core

  xTaskCreatePinnedToCore(
    readBufferTask,
    "readBuffer",
    2048, // Stack size
    NULL, // No parameters
    1,    // Task priority
    NULL, // No task handle needed
    app_cpu); // Run on selected core
}

void loop() {
  // Main loop runs as a FreeRTOS task at priority 1
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
