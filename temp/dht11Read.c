// dht11Read.c

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bcm2835.h"
#include "dht11Read.h"

#define HIGH 0x1
#define LOW 0x0
#define CYCLETIMEOUT 200 // 120us(cycles) expected for binary 1

// Pipes the binary data into variables, separating the temperature and humidity, and comparing the checksum to the data
uint8_t dht11_convert(uint8_t* data, uint8_t* temp, uint8_t* humi) {
    uint8_t check = 0;
    for(int i = 0; i < 8; i++) {
        humi[0] <<= 1;
        humi[0]  += data[i];
        humi[1] <<= 1;
        humi[1]  += data[i+8];
        temp[0] <<= 1;
        temp[0]  += data[i+16];
        temp[1] <<= 1;
        temp[1]  += data[i+24];
        check   <<= 1;
        check    += data[i+32];
    }
    // printf("\n%d.%d%% %d.%d'C %d\n",humi[0],humi[1],temp[0],temp[1],check);
    return (check != (humi[0]+humi[1]+temp[0]+temp[1]));
}

// Converts cycle differences to binary values
uint8_t dht11_binaryData(uint8_t* cycles, uint8_t * buffer){
    for(int i = 0; i < 78; i+=2) {
        if (cycles[i] == CYCLETIMEOUT | cycles[i+1] == CYCLETIMEOUT) {
            return CYCLETIMEOUT;
        }
        buffer[i/2] = (cycles[i] < cycles[i+1]); // if high is longer than low; then 1; else 0;
    }
    return 0;
}

// Counts the number of microseconds that a pin is at a voltage level
uint8_t cycleCount(uint8_t pin, uint8_t level) { // microseconds at pin at given level
    uint8_t count = 0;
    while (bcm2835_gpio_lev(pin) == level) {
        if (count++ >= CYCLETIMEOUT) {
            return CYCLETIMEOUT;
        }
        bcm2835_delayMicroseconds(1);
    }
    return count;
}

/*
    Saves the apparent temperature and humidity to the given variables, returns 1 if errors, fails more than not. Run ~10x
    Does all the timings and convertsions to send the data to the given memory locations at a given pin and pointers
*/
int dht11Read_main(uint8_t pin, uint8_t * temp, uint8_t * humi) {
    uint8_t dataCycles[80];
    uint8_t data[40];

    if (!bcm2835_init()) {
        printf("bcm2835_init failed to initialise\n");
        return 1;
    }

    bcm2835_gpio_set_pud(pin, 0x1); // off:00; pulldown:01; pullup:10;
    bcm2835_gpio_fsel(pin, 0x1); // set as output
    bcm2835_gpio_clr(pin); // set low
    bcm2835_delay(20); // wait >18 ms
    bcm2835_gpio_set(pin); // set high
    bcm2835_delayMicroseconds(30); // wait 20-40 us
    bcm2835_gpio_fsel(pin,0x0); // set as input

    bcm2835_delayMicroseconds(80); // dht pulls low
    bcm2835_delayMicroseconds(80); // dht pulls high

    if(cycleCount(pin, HIGH) == CYCLETIMEOUT) { // wait for low, or exit
        printf("Could not find low after 80,80us delays\n");
        return 1;
    }

    for(int i = 0; i < 80; i += 2){
        dataCycles[i] = cycleCount(pin,LOW);
        dataCycles[i+1] = cycleCount(pin,HIGH);
    }

    if(dht11_binaryData(dataCycles, data)) {
        printf("Timed out during data collection\n");
        // printf("High vs Low -> Bit\n");
        // for (int i = 0; i < 80; i+=2) {
        //     printf("%d vs %d\n -> %d", dataCycles[i], dataCycles[i+1], data[i/2]);
        // }
        return 1;
    }

    if(dht11_convert(data,temp,humi)){
        printf("Checksum does not match, failed to get data correctly\n");
        // printf("\nData: ");
        // for(int i = 0; i < 40; i++) {
        //     printf("%d",data[i]);
        //     if((i+1)%8==0){printf(" ");}
        // }
        return 1;
    }
    return 0;
}
