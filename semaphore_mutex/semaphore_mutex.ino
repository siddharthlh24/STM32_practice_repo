// Using a mutex to prevent function from going out of scope !

// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
TaskHandle_t toggleTaskHandle;

volatile uint32_t toggleDelay = 500u;

static SemaphoreHandle_t mutex;

void displayValTask(void * param)
{
  // So many blasphemies
  uint8_t displayVal = *(uint8_t*)param;

    vTaskDelay(3000/ portTICK_PERIOD_MS);
    Serial.println(displayVal);
    Serial.println(*(uint8_t*)param);

    xSemaphoreGive(mutex);
    vTaskDelay(1000/ portTICK_PERIOD_MS);

}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin,OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000); // delay 1000 ticks
  uint8_t tempVal = 45u;
  mutex = xSemaphoreCreateMutex();
  xSemaphoreTake(mutex, 0u);

  xTaskCreatePinnedToCore(
    displayValTask,
    "displayVal",
    1024, // Stack Size
    (void*)&tempVal, // No params to task
    1, // Task priority
    &toggleTaskHandle,  // Task Handle
    app_cpu); // // run on one core.

  while(xSemaphoreTake(mutex,0) != pdTRUE)
  {
    vTaskDelay(500/ portTICK_PERIOD_MS);
    Serial.println("waiting for release");
    uint8_t a = 1;
    a +=1;
    ; // we wait and run
  }

}

// vTaskStartScheduler() in main needs to get called.
// setup and loop run as priority 1 in arduino

void loop() {
  // put your main code here, to run repeatedly:
}
