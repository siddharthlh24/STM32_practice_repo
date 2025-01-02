
// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
TaskHandle_t toggleTaskHandle;

void toggleTask(void * param)
{
  while(true)
  {
    digitalWrite(led_pin, HIGH);
    // divide ms by tick period to get num ticks
    vTaskDelay(500u / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500u / portTICK_PERIOD_MS);
  }
}

// void toggleTask33u(void * param)
// {
//   while(true)
//   {
//     digitalWrite(led_pin, HIGH);
//     // divide ms by tick period to get num ticks
//     vTaskDelay(333u / portTICK_PERIOD_MS);
//     digitalWrite(led_pin, LOW);
//     vTaskDelay(333u / portTICK_PERIOD_MS);
//   }
// }

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin,OUTPUT);
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    toggleTask,
    "Toggle LED",
    1024, // Stack Size
    NULL, // No params to task
    1, // Task priority
    &toggleTaskHandle,  // Task Handle
    app_cpu); // // run on one core.

  // xTaskCreatePinnedToCore(
  //   toggleTask33u,
  //   "Toggle LED",
  //   1024, // Stack Size
  //   NULL, // No params to task
  //   1, // Task priority
  //   NULL,  // Task Handle
  //   app_cpu); // // run on one core.
}
// vTaskStartScheduler() in main needs to get called.
// setup and loop run as priority 1 in arduino

void loop() {
  // put your main code here, to run repeatedly:

}
