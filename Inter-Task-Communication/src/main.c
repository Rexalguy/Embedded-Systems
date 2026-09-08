#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

// Define the GPIO pin connected to the warning LED (GPIO 2 according to diagram.json)
#define WARNING_LED GPIO_NUM_2

// Define the semaphore handler globally
static SemaphoreHandle_t alertSem = NULL;

// Signaler: Decides when an alert happens
void sensor_alert_task(void *pvParameters) {
    while (1) {
        //Simulate waiting for a condition every 4 seconds
        vTaskDelay(pdMS_TO_TICKS(4000));

        printf("[Sensor] Threshold breached! Triggering warning LED...\n");

        //Give the semaphore - count (0 --> 1)
        xSemaphoreGive(alertSem);
    }
}


// Worker: Waits for the signal to pulse the LED
void led_blink_task(void *pvParameters) {
    while(1) {
        // Blocks indefinitely until the sensor task calls xSemaphoreGive()
        if(xSemaphoreTake(alertSem, portMAX_DELAY) == pdTRUE) {
            //Once the semaphore is received, the LED turns on for 500ms
            gpio_set_level(WARNING_LED,1); //Turn LED ON

            printf("[LED] Warning signal received. Blinking LED for 500ms...\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Keep ON for 500ms
            
            gpio_set_level(WARNING_LED, 0); // Turn LED OFF
        }
        
    }
}

void app_main(void) {

    // 1. Set the mode of the pin
    gpio_set_direction(WARNING_LED, GPIO_MODE_OUTPUT);
    // 1. Initialize the semaphore
    alertSem = xSemaphoreCreateBinary();

    xTaskCreate(sensor_alert_task, "sensor", 2048, NULL, 1, NULL);
    xTaskCreate(led_blink_task,    "led",    2048, NULL, 2, NULL);

}

