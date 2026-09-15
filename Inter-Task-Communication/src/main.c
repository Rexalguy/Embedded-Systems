#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

// Define the connection pin
#define SHARED_LED_PIN GPIO_NUM_4

// 1. Declare the Mutex handle
static SemaphoreHandle_t ledMutex = NULL;

// Task 1: Alert Sequence (Fast Blinks)
void alert_task(void *pvParameters) {
    while(1) {
        //Fires every 4 seconds
        vTaskDelay(pdMS_TO_TICKS(4000));

        printf("[Alert Task] Attempting to claim the status LED...\n");

        // 2. Lock the Mutex (Critical Section Begins)
        if(xSemaphoreTake(ledMutex, portMAX_DELAY) == pdPASS) {
            printf("[Alert Task] >> MUTEX LOCKED: Starting critical 3-pulse alert <<\n");

            // Perform an uninterrupted, atomic 3-blink sequence
            for(int i = 0; i < 3; i++) {
                gpio_set_level(SHARED_LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100)); // On
                gpio_set_level(SHARED_LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(100)); // Off
            }

            printf("[Alert Task] << Alert complete. RELEASING Mutex. >>\n");

            // 3. Unlock the Mutex (Critical Section Ends)
            xSemaphoreGive(ledMutex);
        }
    }
}

// Task 2: Heartbeat Sequence (Long Solid Hold)
void heartbeat_task(void *pvParameters) {
    while(1) {
        // Tries to run every 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("[Heartbeat] Requesting LED for heartbeat pulse...\n");

        if (xSemaphoreTake(ledMutex, portMAX_DELAY) == pdPASS)
        {
            printf("[Heartbeat] >> MUTEX LOCKED: Displaying steady heartbeat pulse <<\n");

            // Hold the LED steady for 1.2 seconds
            gpio_set_level(SHARED_LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(1200));
            gpio_set_level(SHARED_LED_PIN, 0);

            printf("[Heartbeat] << Heartbeat finished. RELEASING Mutex. >>\n");

            xSemaphoreGive(ledMutex);
        }
    }
}

// Main entry point for the ESP-IDF application
void app_main(void){
    // Initialise the pins
    gpio_reset_pin(SHARED_LED_PIN);
    gpio_set_direction(SHARED_LED_PIN, GPIO_MODE_OUTPUT);

    // 4. Create the Mutex
    // Starts in the "Unlocked" (Available) state
    ledMutex = xSemaphoreCreateMutex();
    if(ledMutex == NULL){
        printf("ERROR: Could not create Mutex!\n");
    }

    // Spawn both tasks: Alert has higher priority (2), Heartbeat has lower (1)
    xTaskCreate(heartbeat_task, "heartbeat", 2048, NULL, 1, NULL);
    xTaskCreate(alert_task,     "alert",     2048, NULL, 2, NULL);
} 