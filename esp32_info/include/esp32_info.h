#ifndef __ESP32_INFO_H__
#define __ESP32_INFO_H__

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

void esp32_get_informations();

#endif