// Using a mutex to prevent function from going out of scope !

// use core 0
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;

static TimerHandle_t one_shot_timer = NULL;

void myTimerCallback(TimerHandle_t xTimer)
{
  digitalWrite(led_pin, LOW);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin,OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000); // delay 1000 ticks

  Serial.println("Timer demo test");

  one_shot_timer = xTimerCreate("oneShotTimer", 5000/portTICK_PERIOD_MS, pdFALSE, (void*)0, myTimerCallback);
  if(one_shot_timer == NULL) {
    Serial.println("Failed to create timer");
  }
  digitalWrite(led_pin, HIGH);
  xTimerStart(one_shot_timer, portMAX_DELAY);

}

void loop()
{
  if(Serial.available())
  {
    String data = Serial.readString();
    Serial.println(data);
    digitalWrite(led_pin, HIGH);
    xTimerStart(one_shot_timer, portMAX_DELAY);
  }
}
