#include <wiringPi.h>
#include <stdio.h>

#define sda 27


// pull down for 20ms : 1
// pull up for 20-40us : 2

// dht pulls low for 80us : 3
// dht pulls high for 80us : 4

// data : 
// low for 50us : 5
// high for 26-28 us for 0 : 6
// high for 70us for 1 : 7

// real : 
// low  for 22us : 5
// high for 8us : (0)
// high for 26us : (1)

// encodes as fixed point decimal 8.8 : RH then T then 8 check sum
// most significant first

int pipe(char *);
void bitConverter(char *);


int main() {

    char buffer[40];
    // char rhi[8];
    int rhi;
    char rhd[8];
    char ti[8];
    char td[8];
    char cs[8];

    wiringPiSetup();

    char * pbuffer = &buffer[0];


    pinMode(sda, OUTPUT);
    
    digitalWrite(sda, LOW);
    delay(25); // 1
    
    digitalWrite(sda, HIGH);
    delayMicroseconds(30); // 2
    
    pinMode(sda, INPUT);
    pullUpDnControl(sda, PUD_UP);

    // delayMicroseconds(30); // 3
    // while(!digitalRead(sda)); // testing until == 1

    // delayMicroseconds(70); // 4
    // while(digitalRead(sda)); // testing until goes into data mode; == 0

    for(int i = 0; i < 40; i++) { // data collection
        while(!digitalRead(sda)); // testing until == 1 // 5 low before data

        // bitConverter(pbuffer); // 6|7 // sda = 1
        delayMicroseconds(16); // past 0; in 1
        buffer[i] = digitalRead(sda);
        
        while(digitalRead(sda)); // wait till bit is done

        // pbuffer++; // move to next bit
    }
        // lines goes high after last bit, gets stuck if 0 ??
    // while(!digitalRead(sda)); // testing until == 1 // 5 low before data
    // bitConverter(pbuffer); // 6|7 // sda = 1

    //-----------Converting-----------//


    // rhi = buffer[0] ? (rhi | 128) : rhi;
    // rhi = pipe(&buffer[0]);
    for(int i = 0; i < 40; i++) {
        printf("%i",(buffer[i]));
    }
    // printf("%i", rhi);


    return 0;
}

int pipe(char * pbuffer) {
    int val = 0;
    for(int j=0; j < 8; j++) {
        val = val << 1;
        val = *pbuffer ? val + 1 : val;
        pbuffer++;
    }
    val = val & 0xFF;
    return val;
}

void bitConverter(char* buf) { // 1 : 70us, 0 : 28us, 50us inbetween bits
    delayMicroseconds(16); // past a 0
    *buf = digitalRead(sda);
    // delayMicroseconds();
    // return;
}