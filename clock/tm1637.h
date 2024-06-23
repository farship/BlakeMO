// tm1637.h
#ifndef __TM1637__
#define __TM1637__

// Initialize the tm1637 by specifying the digital pins used to communicate
//
int tm1637Init(unsigned char bClock, unsigned char bData);
//
// Set brightness (0-8)
//
void tm1637SetBrightness(unsigned char b);
//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1637ShowDigits(char *pString);

#endif // __TM1637__