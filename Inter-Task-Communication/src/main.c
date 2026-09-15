#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h" // Required for FreeRTOS Software Timers

// Callback function executed by the FreeRTOS Timer Service (Daemon) Task
void myTimerCallback(TimerHandle_t xTimer) {

    printf("Timer has expired, Executing quick action....\n");
}

void app_main(void) {

    printf("Initializing FreeRTOS Software Timer...\n");

    // 1. Create an Auto-Reload timer set for 1000 ms
    TimerHandle_t myTimer = xTimerCreate(
        "AutoReload",  // Text name for debugging
        pdMS_TO_TICKS(1000), // Timer period: 1000ms = 1s
        pdTRUE,  //pdTRUE = auto-reload | pdFALSE = one-shot
        (void *)0, //Timer ID, useful to manage multiple timers
        myTimerCallback // Timer callback function
    );

    // 2. Start the timer if creation was successful
    if (myTimer != NULL) {

        // Parameter 2 is block time: how long to wait if the timer command queue is full.
        // 0 ticks means return immediately without blocking app_main.
        if (xTimerStart(myTimer, 0) == pdPASS) {
            printf("Timer started sucessfully\n");
        }
        else {
            printf("Failed to start timer!\n");
        }
    }
    else {
        printf("Failed to create timer!\n");
    }

    // app_main finishes and deletes its temporary task, 
    // but the timer continues running in the background via the Timer Daemon!
}