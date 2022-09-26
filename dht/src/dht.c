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


static int dht_await_pin_state(uint32_t timeout,
       int expected_pin_state)
{
    int micros_ticks = 0;
    while(gpio_get_level(dht_gpio) == expected_pin_state) { 
        if(micros_ticks++ > timeout) 
        {
            return DHT11_TIMEOUT_ERROR;
        }
        ets_delay_us(1);
    }
    return micros_ticks;
}

void Send_start_Signal()
{
    // Phase 'A' pulling signal low to initiate read sequence
    //ESP32 set pin DATA as output and set it to 0 at leat 18ms
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20000);

    //ESP32 pull up DATA pin and wait 20-40us
    gpio_set_level(dht_gpio, 1);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
    ets_delay_us(40);
}

static dht_status_t check_response()
{
    // Data pin will in 0 level in 80us
    if(dht_await_pin_state(80, 0) == DHT11_TIMEOUT_ERROR)
    {
        printf("Initialization error, problem in phase 'C' \n");
    }
    // After that DHT pull up the DATA pin in 80us
    if(dht_await_pin_state(80, 1) == DHT11_TIMEOUT_ERROR)
    {
        printf("Initialization error, problem in phase 'D' \n");
    }

    return DHT11_OK;
}

static dht_status_t get_data(uint8_t data[5])
{
    Send_start_Signal();

    check_response();

    // Each bit is sent as a 50us low pulse followed by a variable length high pulse.
    // If the high pulse is ~28us then it is a 0
    // If it is ~70us then it is a 1
    /* Read 40 data bits */
    for(int i = 0; i < 40; i++) {
        if(dht_await_pin_state(50, 0) == DHT11_TIMEOUT_ERROR)
        {
            return DHT11_TIMEOUT_ERROR;
        }

        if(dht_await_pin_state(70, 1) > 28)
        {
            // bit was a 1
            uint8_t b = i / 8;
            uint8_t m = i % 8;
            if (!m)
            data[b] = 0;
            data[b] |= (1) << (7 - m);;
        }
        else{
            // No action for bit 0
        }
    }

    return DHT11_OK;
}

dht11_reading_t read_value()
{
    uint8_t data[DHT_DATA_BYTES] = { 0 };
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht_gpio, 1);

    if (get_data(data) == DHT11_TIMEOUT_ERROR)
    {
        read_data.status = DHT11_GET_DATA_FAILED;
        return read_data;
    }

    /* restore GPIO direction because, after calling get_data(), the
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
