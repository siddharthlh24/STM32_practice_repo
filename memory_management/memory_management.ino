// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void testTask(void * param)
{
  while(1)
  {
    int a = 1;
    int *b = (int*)pvPortMalloc(100*sizeof(int));

    if(b!=NULL){
          memset(b,0,100*sizeof(int));

    for(int i=1;i<100;i++)
    {
      b[i] = a+i;
    }
    Serial.println(b[0]);

    }
    vTaskDelay(10);
    Serial.println(xPortGetFreeHeapSize());

  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000);
  Serial.println("tsrating demo");
  xTaskCreatePinnedToCore(testTask, "testTask", 2048, NULL, 1, NULL, app_cpu);

  // no need to start anything
  vTaskDelete(NULL);

}

void loop() {
  // put your main code here, to run repeatedly:

}
