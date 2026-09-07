#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define LED pins
#define LED1_PIN 2
#define LED2_PIN 4
#define LED3_PIN 5

// Task 1: Blink LED1 every 500ms
void led1_task(void *pvParameter) {
    //Pin setup
    gpio_reset_pin(LED1_PIN);
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        gpio_set_level(LED1_PIN, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(LED1_PIN, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// Task 2: Blink LED2 every 1000ms
void led2_task(void *pvParameter) {
    //Pin setup
    gpio_reset_pin(LED2_PIN);
    gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        gpio_set_level(LED2_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED2_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Task 3: Blink LED3 every 1500ms
void led3_task(void *pvParameter) {
    //Pin setup
    gpio_reset_pin(LED3_PIN);
    gpio_set_direction(LED3_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        gpio_set_level(LED3_PIN, 1);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        gpio_set_level(LED3_PIN, 0);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    printf("Starting LED tasks...\n");
    
    // Create 3 tasks with different priorities
    // LED1: 2048 stack, priority 1
    xTaskCreate(led1_task, "LED1_TASK", 2048, NULL, 1, NULL);
    //LED2: 2048 stack, priority 1
    xTaskCreate(led2_task, "LED2_TASK", 2048, NULL, 1, NULL);
    //LED3: 2048 stack, priority 1
    xTaskCreate(led3_task, "LED3_TASK", 2048, NULL, 1, NULL);
    
    printf("LED tasks created successfully\n");
}