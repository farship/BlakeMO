// dht11Read.h

#include <stdint.h>

uint8_t dht11_convert(uint8_t* data, uint8_t* temp, uint8_t* humi);

uint8_t dht11_binaryData(uint8_t* cycles, uint8_t * buffer);

uint8_t cycleCount(uint8_t pin, uint8_t level); // microseconds at pin at given level

/*
    RUN 10X WAITING FOR A RETURN OF 0, WITH 1-2 SECOND DELAY
    Read the Temperature and Humidity using a DHT11 sensor
    pin number defined for the bcm2835 same as gpio numbering
    temp is a pointer to a uint8_t[2] : 8.8 fixed point decimal, C
    humi is a pointer to a uint8_t[2] : 8.8 fixed point decimal, %
*/
int dht11Read_main(uint8_t pin, uint8_t * temp, uint8_t * humi); 