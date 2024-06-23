#include <stdio.h>
#include <stdlib.h>
// #include <bcm2835.h>
#include <wiringPi.h>
#include <time.h>
#include "tm1637.h"

#define CLK 2
#define DIO 3

// TM1637Display display(CLK, DIO);

int getTime();

int main () 
{
    short int hour = 0;
    short int min = 0;
    char cTime[6]; // 12:45\6

    wiringPiSetup();
    delay(10);
    // int display wiringPiI2CSetup(0x27);
    tm1637Init(CLK, DIO);
    tm1637SetBrightness(8);
   //  tm1637ShowDigits("99:99");

    // Display current time
    if (!getTime(&hour, &min)) {
        printf("Current Time: %d:%d \n", hour, min);
        sprintf(cTime, "%02d:%02d", hour, min);
    }
    tm1637ShowDigits(cTime);
    return 0;
}

int getTime(int *phour, int *pmin) {
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    *pmin = timeInfo->tm_min;
    *phour = timeInfo->tm_hour;
    return 0;
}
