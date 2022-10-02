#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp32_info.h"

void test_esp32_info()
{
    while(1)
    {
        esp32_get_informations();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
void app_main()
{
    xTaskCreate(test_esp32_info, "test", 2048 * 3, NULL, 5, NULL);
}