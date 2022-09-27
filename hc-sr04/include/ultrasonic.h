#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include <freertos/FreeRTOS.h>
#include <string.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include "esp_timer.h"
#include "freertos/task.h"
#include <driver/gpio.h>

gpio_num_t sensor_pin;
#define MAX_DISTANCE_CM 500
#define PING_TIMEOUT 6000
#define ROUNDTRIP_M 5800.0f
#define ROUNDTRIP_CM 58
#define MAX_TIME_US (MAX_DISTANCE_CM * ROUNDTRIP_CM)
typedef enum
{
    ERR_ULTRASONIC_MEAS_FAILED = -4,
    ERR_ULTRASONIC_PING,
    ERR_ULTRASONIC_PING_TIMEOUT,
    ERR_ULTRASONIC_PING_ECHO_TIMEOUT,
    ULTRASONIC_INIT,
    ULTRASONIC_OK,
} ultrasonic_status_t;

typedef struct ultrasonic_sensor
{
    gpio_num_t echo_pin;
    gpio_num_t trig_pin;
    float distance_m;
    float distance_cm;
    ultrasonic_status_t status;
} ultrasonic_sensor_t;

ultrasonic_sensor_t sensor;

void ultrasonic_init(gpio_num_t echo_pin, gpio_num_t trig_pin);

ultrasonic_sensor_t ultrasonic_measure_raw();


#endif