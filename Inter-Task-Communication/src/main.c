#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define GPIO pin mapping for segments A through G
#define SEG_A GPIO_NUM_4
#define SEG_B GPIO_NUM_5
#define SEG_C GPIO_NUM_6
#define SEG_D GPIO_NUM_7
#define SEG_E GPIO_NUM_15
#define SEG_F GPIO_NUM_16
#define SEG_G GPIO_NUM_17


// Combine all segment pins into one single 64-bit mask for gpio_config()
#define PIN_BIT_MASK ( (1ULL << SEG_A) | \
                       (1ULL << SEG_B) | \
                       (1ULL << SEG_C) | \
                       (1ULL << SEG_D) | \
                       (1ULL << SEG_E) | \
                       (1ULL << SEG_F) | \
                       (1ULL << SEG_G) )

// Bit patterns for digits 0-9 (Common Cathode: 1 = ON, 0 = OFF)
// Format: 0b0(g)(f)(e)(d)(c)(b)(a)
static const uint8_t digit_patterns[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

/**
 * @brief Sends segment levels to the 7 pins using right-shifts and bitwise AND
 */
void display_digits(uint8_t num) {

    if (num > 9) {
        return;
    }

    uint8_t pattern = digit_patterns[num];

    // Isolate each bit and write directly using native ESP-IDF API
    gpio_set_level(SEG_A, (pattern >> 0) & 0x01);
    gpio_set_level(SEG_B, (pattern >> 1) & 0x01);
    gpio_set_level(SEG_C, (pattern >> 2) & 0x01);
    gpio_set_level(SEG_D, (pattern >> 3) & 0x01);
    gpio_set_level(SEG_E, (pattern >> 4) & 0x01);
    gpio_set_level(SEG_F, (pattern >> 5) & 0x01);
    gpio_set_level(SEG_G, (pattern >> 6) & 0x01);
}

/**
 * @brief FreeRTOS Task cycling through 0 to 9
 */


 void segment_counter_task(void *pvParameters)
{
    uint8_t current_digit = 0;

    while (1)
    {
        printf("Current Digit: %d (Pattern: 0x%02X)\n", current_digit, digit_patterns[current_digit]);
        display_digits(current_digit);

        // Advance to next digit
        current_digit = (current_digit + 1) % 10;

        // Block task for 1 second (0% CPU)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // 1. One-time hardware configuration for all 7 pins at once
    gpio_config_t io_conf = {
        .pin_bit_mask = PIN_BIT_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // 2. Start counter FreeRTOS task
    xTaskCreate(segment_counter_task, "sevseg_task", 2048, NULL, 1, NULL);
}
