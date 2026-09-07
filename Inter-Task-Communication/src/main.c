#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Declare the Queue globally
static QueueHandle_t sensorQueue = NULL;

// Producer: The sending task
void sender_task(void *pvParameters){
    int temp = 20;

    while(1) {
        printf("[Producer] sendind temp: %d \n", temp);

        // xQueueSend: copies the value of 'temperature' into the queue.
        // portMAX_DELAY tells the task to block forever if the queue is full.
        BaseType_t status = xQueueSend(sensorQueue, &temp, pdMS_TO_TICKS(1000));

        if(status != pdPASS) {
            printf("Failed to send to queue\n");
        }
        temp++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

// Consumer: Receiving task

void customer_task(void *pvParameters) {
    int received_temp = 0;

    while(1) {
        // xQueueReceive: copied the value from the queue to received_temp
        // Blocks until an item is available.
        BaseType_t status = xQueueReceive(sensorQueue, &received_temp, pdMS_TO_TICKS(1000));

        if(status == pdPASS) {
            printf("[Consumer] received temp: %d \n", received_temp);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    // Create the queue: max 5 integers, each integer takes 4 bytes
    sensorQueue = xQueueCreate(5, sizeof(int));
    
    if (sensorQueue == NULL) 
    {
        printf("Queue creation failed! Not enough heap memory.\n");
        return;
    }

    // Create the two tasks
    xTaskCreate(sender_task, "producer", 2048, NULL, 1, NULL);
    xTaskCreate(customer_task, "customer", 2048, NULL, 2, NULL);
      

}