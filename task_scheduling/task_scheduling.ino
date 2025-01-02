
// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
TaskHandle_t toggleTaskHandle;

volatile uint32_t toggleDelay = 500u;

void toggleTask(void * param)
{
  while(true)
  {
    digitalWrite(led_pin, HIGH);
    // divide ms by tick period to get num ticks
    vTaskDelay(toggleDelay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(toggleDelay / portTICK_PERIOD_MS);
  }
}

void setDelayTask(void * param)
{
  while (true)
  {
    if (Serial.available() > 0)
    {
      String buffer = Serial.readStringUntil('\n'); // Read until newline
      uint32_t newDelay = (uint32_t)buffer.toInt(); // Convert to integer
      if (newDelay > 0 && newDelay <= 5000) // Validate delay (1ms to 5000ms)
      {
        toggleDelay = newDelay;
        Serial.print("Delay updated to: ");
        Serial.println(toggleDelay);
      }
      else
      {
        Serial.println("Invalid delay. Enter a value between 1 and 5000.");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Check input every 100ms
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin,OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000); // delay 1000 ticks

  xTaskCreatePinnedToCore(
    toggleTask,
    "Toggle LED",
    1024, // Stack Size
    NULL, // No params to task
    1, // Task priority
    &toggleTaskHandle,  // Task Handle
    app_cpu); // // run on one core.

    xTaskCreatePinnedToCore(
    setDelayTask,
    "SetDelay",
    1024, // Stack Size
    NULL, // No params to task
    1, // Task priority
    NULL,  // Task Handle
    app_cpu); // // run on one core.
}

// vTaskStartScheduler() in main needs to get called.
// setup and loop run as priority 1 in arduino

void loop() {
  // put your main code here, to run repeatedly:
}
