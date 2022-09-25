#include "dht.h"
#include <freertos/FreeRTOS.h>
#include <string.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include "esp_timer.h"
#include "freertos/task.h"
/*
 *  Note:
 *  A suitable pull-up resistor should be connected to the selected GPIO line
 *
 *  __           ______          _______                              ___________________________
 *    \    A    /      \   C    /       \   DHT duration_data_low    /                           \
 *     \_______/   B    \______/    D    \__________________________/   DHT duration_data_high    \__
 *
 *
 *  Initializing communications with the DHT requires four 'phases' as follows:
 *
 *  Phase A - MCU pulls signal low for at least 18000 us
 *  Phase B - MCU allows signal to float back up and waits 20-40us for DHT to pull it low
 *  Phase C - DHT pulls signal low for ~80us
 *  Phase D - DHT lets signal float back up for ~80us
 *
 *  After this, the DHT transmits its first bit by holding the signal low for 50us
 *  and then letting it float back high for a period of time that depends on the data bit.
 *  duration_data_high is shorter than 50us for a logic '0' and longer than 50us for logic '1'.
 *
 *  There are a total of 40 data bits transmitted sequentially. These bits are read into a byte array
 *  of length 5.  The first and third bytes are humidity (%) and temperature (C), respectively.  Bytes 2 and 4
 *  are zero-filled and the fifth is a checksum such that:
 *
 *  byte_5 == (byte_1 + byte_2 + byte_3 + byte_4) & 0xFF
 *
 */

gpio_num_t dht_gpio;

void DHT11_init(gpio_num_t pin)
{
    /* Wait 1 seconds to make the device pass its initial unstable status */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = pin;
    //return DHT11_OK;
}


static dht_status_t dht_await_pin_state(gpio_num_t pin, uint32_t timeout,
       int expected_pin_state, uint32_t *duration)
{
    /* XXX dht_await_pin_state() should save pin direction and restore
     * the direction before return. however, the SDK does not provide
     * gpio_get_direction().
     */
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    for (uint32_t i = 0; i < timeout; i += DHT_TIMER_INTERVAL)
    {
        // need to wait at least a single interval to prevent reading a jitter
        ets_delay_us(DHT_TIMER_INTERVAL);
        if (gpio_get_level(pin) == expected_pin_state)
        {
            if (duration)
                *duration = i;
            return DHT11_OK;
        }
    }

    return DHT11_TIMEOUT_ERROR;
}

static dht_status_t check_response()
{
    // Phase 'A' pulling signal low to initiate read sequence
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20000);
    gpio_set_level(dht_gpio, 1);

    if(dht_await_pin_state(dht_gpio, 40, 0, NULL) == DHT11_TIMEOUT_ERROR)
    {
        printf("Initialization error, problem in phase 'B' \n");
    }

    if(dht_await_pin_state(dht_gpio, 88, 1, NULL) == DHT11_TIMEOUT_ERROR)
    {
        printf("Initialization error, problem in phase 'C' \n");
    }

    if(dht_await_pin_state(dht_gpio,88, 0, NULL) == DHT11_TIMEOUT_ERROR)
    {
        printf("Initialization error, problem in phase 'D' \n");
    }

    return DHT11_OK;
}

static dht_status_t get_data(uint8_t data[5])
{
    uint32_t low_duration;
    uint32_t high_duration;


    check_response();

    /* Read 40 data bits */
    for(int i = 0; i < 40; i++) {
        if(dht_await_pin_state(dht_gpio, 65, 1, &low_duration) == DHT11_TIMEOUT_ERROR)
        {
            printf("Low bit time out \n");
        }

        if(dht_await_pin_state(dht_gpio, 75, 0, &high_duration) == DHT11_TIMEOUT_ERROR)
        {
            printf("Hight bit time out' \n");
        }

        uint8_t b = i / 8;
        uint8_t m = i % 8;
        if (!m)
            data[b] = 0;

        data[b] |= (high_duration > low_duration) << (7 - m);
    }

    return DHT11_OK;
}

dht11_reading_t read_value()
{
    uint8_t data[DHT_DATA_BYTES] = { 0 };
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht_gpio, 1);

    dht_status_t result = get_data(data);

    /* restore GPIO direction because, after calling dht_fetch_data(), the
     * GPIO direction mode changes */
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht_gpio, 1);

     if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        read_data.humidity = -1;
        read_data.temperature = -1;
        read_data.status = DHT11_CRC_ERROR;
        printf("Checksum failed, invalid data received from sensor\n");
    }
    else{
        read_data.humidity = data[2];
        read_data.temperature = data[0];
        read_data.status = DHT11_OK;
        printf("Checksum success, valid data received from sensor\n");
    }

    //printf("Sensor data: humidity=%d, temp=%d \n", hum, temp);
    return read_data;
}
