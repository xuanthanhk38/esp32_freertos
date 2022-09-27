#include "ultrasonic.h"

#define TRIGGER_GPIO 17
#define ECHO_GPIO 16

void ultrasonic_test(void *param)
{
    ultrasonic_init(ECHO_GPIO, TRIGGER_GPIO);

    while(1)
    {
        ultrasonic_measure_raw();
        printf("Distance in cm: %0.04f \n", sensor.distance_cm);
        printf("Distance in m: %0.04f \n", sensor.distance_m);
        printf("Status: %d \n", sensor.status);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void app_main()
{
    xTaskCreate(ultrasonic_test, "test", 2048*5, NULL, 1, NULL);
}