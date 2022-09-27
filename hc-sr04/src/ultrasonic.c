#include "ultrasonic.h"

//check timeout
#define check_timeout(time, check_time) (esp_timer_get_time() - (start) >= check_time)

void ultrasonic_init(gpio_num_t echo_pin, gpio_num_t trig_pin)
{
    sensor.echo_pin = echo_pin;
    sensor.trig_pin = trig_pin;
    sensor.status = ULTRASONIC_INIT;
    gpio_set_direction(sensor.echo_pin, GPIO_MODE_INPUT);
    gpio_set_direction(sensor.trig_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(sensor.trig_pin, 0);
}

ultrasonic_sensor_t ultrasonic_measure_raw()
{
    uint32_t  time_us;
    // Ping low for 4us
    gpio_set_level(sensor.trig_pin, 0);
    ets_delay_us(4);

    // Ping high for 10us
    gpio_set_level(sensor.trig_pin, 1);
    ets_delay_us(10);

    // set trig_pin to low
    gpio_set_level(sensor.trig_pin, 0);

    if (gpio_get_level(sensor.echo_pin))
    {
        sensor.distance_m = -1;
        sensor.distance_cm = -1;
        sensor.status = ERR_ULTRASONIC_PING;
    }

    int64_t start = esp_timer_get_time();

    while (!gpio_get_level(sensor.echo_pin))
    {
        if(check_timeout(start, PING_TIMEOUT))
        {
            sensor.distance_m = -1;
            sensor.distance_cm = -1;
            sensor.status = ERR_ULTRASONIC_PING_TIMEOUT;
        }
    }

    //got echo, measuring
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;



    // echo pulse width corespond to distance
    while (gpio_get_level(sensor.echo_pin))
    {
        time = esp_timer_get_time();
        if(check_timeout(echo_start, echo_start + MAX_TIME_US))
        {
            sensor.distance_m = -1;
            sensor.distance_cm = -1;
            sensor.status = ERR_ULTRASONIC_PING_ECHO_TIMEOUT;
        }
    }
    // from echo start to echo back
    time_us = time - echo_start;

    //distance in M
    sensor.distance_m = time_us / ROUNDTRIP_M;
    sensor.distance_cm = time_us / ROUNDTRIP_CM;
    sensor.status = ULTRASONIC_OK;

    return sensor;


}