// Use core 0 or 1 based on configuration
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const uint8_t max_queue_len = 5u;
static QueueHandle_t msg_queue;
// put your main code here, to run repeatedly:
static int num = 0;

void printMessages(void * param)
{
  int item;

  while(true)
  {
    if(xQueueReceive(msg_queue, (void*)&item, 0u) == pdTRUE) {
      Serial.println(item);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("RTOS Queue TEST");
  Serial.println("RTOS Queue TEST");

  msg_queue = xQueueCreate(5, sizeof(int));

  xTaskCreatePinnedToCore(printMessages, "printMessage", 2048, NULL, 1, NULL, app_cpu);

}

void loop() {
  if(xQueueSend(msg_queue,(void*)&num,10)!= pdTRUE) {
    Serial.println("Queue full");
  }
  num++;
  vTaskDelay(500/portTICK_PERIOD_MS);

}
