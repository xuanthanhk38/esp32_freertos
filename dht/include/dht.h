/**
 * ESP-IDF driver for DHT11
 * 
 */
#ifndef __DHT_H__
#define __DHT_H__

#include <driver/gpio.h>
#include <esp_err.h>



typedef enum dht_status{
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
} dht_status_t;

// dht timer precision in microseconds
#define DHT_TIMER_INTERVAL 2

// a complete data transmission is 40 bit
#define DHT_DATA_BITS 40

/* The first and third bytes are humidity (%) and temperature (C), respectively.
 *  Bytes 2 and 4 are zero-filled and the fifth is a checksum such that:
 *  byte_5 == (byte_1 + byte_2 + byte_3 + byte_4) & 0xFF
 */
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

typedef struct dht11_reading {
    int status;
    int temperature;
    int humidity;
} dht11_reading_t;

dht11_reading_t read_data;
void DHT11_init(gpio_num_t);
//static uint32_t dht_check_pin_state(uint32_t timeout, int expected_pin_state);

//static esp_err_t dht_transmission_process();
//static esp_err_t check_response();
//static int _checkCRC(uint8_t data[]);

struct dht11_reading dht_reading();
dht11_reading_t read_value();

//static struct dht11_reading last_read;
//static esp_err_t dht_init(gpio_num_t pin);
#endif //__DHT_H_