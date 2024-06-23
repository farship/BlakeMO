#include "dht11Read.h"
#include "bcm2835.h"
#include <stdio.h>

int main(){
    uint8_t oldt[2];
    uint8_t t[2];
    uint8_t oldh[2];
    uint8_t h[2];

    while(1){
    // for(int i = 0; i < 10; i++) {
        t[0] = 0; t[1] = 0;
        h[0] = 0; h[1] = 0;
        // printf("\n%d\n",i);
        if (!dht11Read_main(14, &t[0], &h[0]) & h[0]<128 & h[1]==0) {
            oldt[0] = t[0]; oldt[1] = t[1];
            oldh[0] = h[0]; oldh[1] = h[1];
            // break;
            // WRITE CURRENT VALUES TO FILES SO THAY CAN READ BY THE REST OF THE OS
            // only when they change and are valid
        }
        printf("\nCurrent T/H : %d.%d'C | %d%%\n",oldt[0],oldt[1],oldh[0]);
        bcm2835_delay(2500);
    }
    // printf("\n");
    // printf("Humidity:    %i.%i %%\n",h[0],h[1]);
    // printf("Temperature: %i.%i `C\n",t[0],t[1]);
}