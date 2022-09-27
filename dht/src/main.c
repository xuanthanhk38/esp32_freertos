#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"


#include "dht.h"

void app_main()
{

    DHT11_init(GPIO_NUM_16);

    while(1) {
        read_value();
        printf("Temperature is %d \n", read_data.temperature);
        printf("Humidity is %d\n", read_data.humidity);
        printf("Status code is %d\n", read_data.status);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}