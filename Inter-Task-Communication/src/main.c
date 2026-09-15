#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 1. GLOBAL TASK HANDLE:
// Holds the memory address of worker_task's Task Control Block (TCB).
// Task Notifications write directly into the recipient's TCB, so the sender
// only needs this handle to deliver signals—no queue or semaphore required.
TaskHandle_t workerTaskHandle = NULL;

/**
 * @brief Sender Task (Producer)
 * 
 * Runs at Priority 1 (lower priority).
 * Periodically wakes up, dispatches a direct notification to worker_task,
 * and yields execution.
 */

void sender_task(void *pvParameters) {

    while(1) {
        // Puts this task into the Blocked state for 1000 ms.
        // It consumes 0% CPU cycles while sleeping.
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("Sender: Sending notification to worker task...\n");
        // Send direct notification to workerTask:
        // Under the hood, this increments worker_task's internal 32-bit notification value
        // and transitions worker_task from Blocked -> Ready.
        //
        // Because worker_task runs at Priority 2 (higher than this task's Priority 1),
        // the FreeRTOS scheduler immediately preempts sender_task mid-execution
        // to run worker_task without delay.
        xTaskNotifyGive(workerTaskHandle);
    }
}

/**
 * @brief Worker Task (Consumer)
 * 
 * Runs at Priority 2 (higher priority).
 * Waits indefinitely in the Blocked state until a notification arrives,
 * processes the event, and goes back to sleep.
 */

void worker_task(void *pvParameters)
{
    while (1)
    {
        // Block until notified:
        // - pdTRUE: Emulates a Binary Semaphore. It resets the internal 32-bit
        //           notification counter back to 0 as soon as the task wakes up.
        // - portMAX_DELAY: Wait indefinitely without timeout. The task consumes
        //                  0% CPU while blocked here.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // This line only executes once sender_task has called xTaskNotifyGive()
        printf("Worker: Notification received! Processing event...\n");

        // After this line, the loop repeats and hits ulTaskNotifyTake() again.
        // Because pdTRUE cleared the counter to 0, it immediately blocks again.
    }
}

void app_main(void)
{
    // 1. Create the Worker Task (Consumer):
    // - Priority: 2 (Higher priority so it handles signals the instant they arrive).
    // - &workerTaskHandle: FreeRTOS assigns the pointer to the newly allocated
    //   Task Control Block (TCB) here so sender_task knows where to target signals.
    xTaskCreate(worker_task, "Worker", 2048, NULL, 2, &workerTaskHandle);

    // 2. Create the Sender Task (Producer):
    // - Priority: 1 (Lower priority).
    // - NULL: No handle is stored because no other task needs to notify sender_task.
    xTaskCreate(sender_task, "Sender", 2048, NULL, 1, NULL);
}
