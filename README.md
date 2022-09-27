# esp32_freertos
ESP32 FreeRTOS learning

1. Hardware issue related to Ultrasonic example: 
It seems to be a badly designed clone, the labels might be wrong (because it is a Chinese board with a price that is half of the market :))
- Reproduce:
  + In example ultrasonic, use pin GND (next to 5V pin) to connect with GND of sensor
  
- Message error: A fatal error occurred: Packet content transfer stopped (received 8 bytes) --> ESP32 Can not upload code

- Solution: use other GND pins except for the GND pin next to the 5V pin

- How: Analyze the ESP32 Devkit v4 pin out,it has GND pin next to 5V pin
  + This GND pin is wired with GPIO11 and this is integrated with SPI flash on the chip
  + and it is not recommended for others use.
  + Check this link for IO: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
