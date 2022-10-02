#include "esp32_info.h"

void esp32_get_informations()
{
    printf("Get ESP32 informations \n");

        esp_chip_info_t chip_info;
        uint32_t flash_size = 0;
        esp_chip_info(&chip_info);

        printf("This is %s esp32 chip with %d cores, Wifi%s%s",
                CONFIG_IDF_TARGET,
                chip_info.cores,
                (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

        printf("silicon revision %d, ", chip_info.revision);
        if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
            printf("Get flash size failed");
            return;
        }

        printf("%uMB %s flash\n", flash_size / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
        printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
}