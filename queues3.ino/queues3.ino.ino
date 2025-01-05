#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Use core 0 or 1 based on configuration
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const uint8_t max_queue_len = 5u;
static QueueHandle_t msg_queue;

typedef struct
{
    uint8_t blinkyDelay; // Blink delay in seconds
    char buff[25];       // Message to repeat
} queuePayload_t;

void coreTask(void *pvParameters)
{
    queuePayload_t receivedMessage;

    for (;;)
    {
        // Wait for a message from the queue
        if (xQueueReceive(msg_queue, &receivedMessage, portMAX_DELAY) == pdPASS)
        {
            // Display the received message
            Serial.print("Received Message: ");
            Serial.println(receivedMessage.buff);
            Serial.print("Blinky Delay: ");
            Serial.print(receivedMessage.blinkyDelay);
            Serial.println(" seconds");

            // Blink the LED 5 times with the specified delay
            for (int i = 0; i < 5; i++)
            {
                // Print the message to Serial each time the LED blinks
                Serial.print("Repeating Message: ");
                Serial.println(receivedMessage.buff);

                digitalWrite(LED_BUILTIN, HIGH);
                vTaskDelay(receivedMessage.blinkyDelay * 1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_BUILTIN, LOW);
                vTaskDelay(receivedMessage.blinkyDelay * 1000 / portTICK_PERIOD_MS);
            }
        }
    }
}

void setup()
{
    // Initialize Serial and LED
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    // Wait for Serial to initialize
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("RTOS Queue TEST");

    // Create the queue
    msg_queue = xQueueCreate(max_queue_len, sizeof(queuePayload_t));
    if (msg_queue == NULL)
    {
        Serial.println("Queue creation failed");
        while (1); // Halt if the queue cannot be created
    }

    // Create the core task
    xTaskCreatePinnedToCore(coreTask, "processMessages", 2048, NULL, 1, NULL, app_cpu);
}

void loop()
{
    if (Serial.available())
    {
        queuePayload_t message;

        // Read input from Serial
        size_t len = Serial.readBytesUntil('\n', message.buff, sizeof(message.buff) - 1);
        message.buff[len] = '\0'; // Null-terminate the string

        // Example: Parse delay from the input message
        // Format expected: "message:3", where 3 is the delay in seconds
        char *colon = strchr(message.buff, ':');
        if (colon != NULL)
        {
            *colon = '\0'; // Split the string at the colon
            message.blinkyDelay = atoi(colon + 1); // Convert the delay to an integer
        }
        else
        {
            message.blinkyDelay = 1; // Default delay
        }

        // Send the message to the queue
        if (xQueueSend(msg_queue, &message, portMAX_DELAY) != pdPASS)
        {
            Serial.println("Failed to send message to the queue");
        }
    }
}
