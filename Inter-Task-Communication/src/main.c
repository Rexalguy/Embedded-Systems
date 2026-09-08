#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

// Define the connection pins
#define LED_GREEN GPIO_NUM_4
#define LED_BLUE GPIO_NUM_5

// Global handle for the counting semaphore
static SemaphoreHandle_t  ledSem = NULL;

// Worker function that will be used to create the competing tasks
void worker_task(void *pvParameters) {

    int task_id = (int)pvParameters;  // Takes the task ID from the argument passed during task creation

    while(1){
        // Tasks request LEDs at staggered intervals
        vTaskDelay(pdMS_TO_TICKS(1500 * task_id));

        printf("[Task %d] Needs an LED to show activity. Checking availability...\n", task_id);

        // Try to take 1 token from the pool.
        // If count > 0, decrements count and proceeds immediately.
        // If count == 0, task enters Blocked state until someone calls xSemaphoreGive().

        if(xSemaphoreTake(ledSem, portMAX_DELAY) == pdTRUE) {
            printf("[Task %d] >> ACQUIRED an LED! (Led count decremented)\n", task_id);

            gpio_num_t acquired_pin = (task_id %2 == 0) ? LED_BLUE : LED_GREEN;
            gpio_set_level(acquired_pin, 1);

            // Hold the resource for 3 seconds of "work"
            vTaskDelay(pdMS_TO_TICKS(3000));

            // Done with the LED
            gpio_set_level(acquired_pin, 0);
            printf("[Task %d] << FINISHED work. Returning LED to pool.\n", task_id);

            // Return 1 token to the pool (increments count)
            xSemaphoreGive(ledSem);
        }
    }
} 

void app_main(void) {
    // Init GPIOs
    gpio_reset_pin(LED_GREEN);
    gpio_reset_pin(LED_BLUE);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);

    // Create counting semaphore:
    // Parameter 1: max_count (Maximum capacity = 2)
    // Parameter 2: initial_count (Available right now = 2)
    ledSem = xSemaphoreCreateCounting(2,2);

    if(ledSem == NULL) {
        printf("Failed to create counting semaphore");
        return;

    }

    //We create the 3 competing tasks
    xTaskCreate(worker_task, "worker1", 2048, (void *)1, 1, NULL);
    xTaskCreate(worker_task, "worker2", 2048, (void *)2, 1, NULL);
    xTaskCreate(worker_task, "worker3", 2048, (void *)3, 1, NULL);
}