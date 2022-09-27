/* Blink Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_GPIO_4 GPIO_ID_PIN(4)
#define BLINK_GPIO_16 GPIO_ID_PIN(17)

void blink_pin4_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    
    gpio_pad_select_gpio(BLINK_GPIO_4);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_4, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        printf("task blink pin 4 is running");
        gpio_set_level(BLINK_GPIO_4, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO_4, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void blink_pin16_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO_16);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_16, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (outpu
        t low) */
        printf("task blink pin 16 is running");
        gpio_set_level(BLINK_GPIO_16, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO_16, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    TaskHandle_t Task1;
    TaskHandle_t Task2;
    xTaskCreatePinnedToCore(blink_pin4_task, "blink_pin4_task", 1024*4, NULL, 5, &Task1, 0);
    xTaskCreatePinnedToCore(blink_pin16_task, "blink_pin16_task", 1024*4, NULL, 5, &Task2, 1);
}