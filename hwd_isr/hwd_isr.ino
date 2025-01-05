// Using a mutex to prevent function from going out of scope !

// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;

static const uint16_t timer_divider = 8;
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000 / portTICK_PERIOD_MS;

static hw_timer_t *timer = NULL;
static volatile int isr_counter;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  // int  pinstate = digitalRead(led_pin);
  // digitalWrite(led_pin,!pinstate);

  portENTER_CRITICAL(&spinlock);
  isr_counter++;
  portEXIT_CRITICAL(&spinlock);
}

void printValues (void * param){
  while(true)
  {
    while(isr_counter>0)
    {
        Serial.println(isr_counter);
        portENTER_CRITICAL(&spinlock);
        isr_counter--;
        portEXIT_CRITICAL(&spinlock);

    }
    vTaskDelay(task_delay);
  }
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);

  timer = timerBegin(0,timer_divider,true);
  timerAttachInterrupt(timer,&onTimer,true);
  timerAlarmWrite(timer,timer_max_count,true);
  timerAlarmEnable(timer);

  xTaskCreatePinnedToCore(printValues, "printValues", 1024, NULL, 1, NULL, app_cpu);

  vTaskDelete(NULL);

}

void loop() {
  // put your main code here, to run repeatedly:

}
