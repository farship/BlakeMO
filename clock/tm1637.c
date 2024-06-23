// tm1637.c
#include <stdio.h>
#include <unistd.h>
// #include <armbianio.h>
#include "tm1637.h"
//
//
const unsigned char cDigit2Seg[] = {0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
static unsigned char bClockPin, bDataPin;
#define CLOCK_DELAY 5
#define LOW 0
#define HIGH 1
//
// Initialize the tm1637 by specifying the digital pins used to communicate
// returns 0 for success, -1 for failure
// 
int tm1637Init(unsigned char bClock, unsigned char bData)
{
// int rc;

//    rc = AIOInit();
//    if (rc != 1)
//    {
// 	   printf("Error initializing ArmbianIO library\n");
// 	   return -1;
//    }
   bClockPin = bClock;
   bDataPin = bData;
   pinMode(bClockPin, OUTPUT);
   pinMode(bDataPin, OUTPUT);
   digitalWrite(bClockPin, LOW);
   digitalWrite(bDataPin, LOW);
   return 0;
} /* tm1637Init() */
//
// Start wire transaction
//
static void tm1637Start(void)
{
   digitalWrite(bDataPin, HIGH);
   digitalWrite(bClockPin, HIGH);
   usleep(CLOCK_DELAY);
   digitalWrite(bDataPin, LOW);
} /* tm1637Start() */
//
// Stop wire transaction
//
static void tm1637Stop(void)
{
  digitalWrite(bClockPin, LOW); // clock low
  usleep(CLOCK_DELAY);
  digitalWrite(bDataPin, LOW); // data low
  usleep(CLOCK_DELAY);
  digitalWrite(bClockPin, HIGH); // clock high
  usleep(CLOCK_DELAY);
  digitalWrite(bDataPin, HIGH); // data high
} /* tm1637Stop() */
//
// Get data ack
//
static unsigned char tm1637GetAck(void)
{
unsigned char bAck = 1;

//
// With the sysfs GPIO driver, ArmbianIO is not prepared to reverse
// the direction of an existing GPIO pin
// We can safely disable this code on Linux
//
 // read ack
   digitalWrite(bClockPin, LOW); // clock to low
//   pinMode(bDataPin, GPIO_IN); // data as input
   usleep(CLOCK_DELAY);
   
//   bAck = AIOReadGPIO(bDataPin);
   digitalWrite(bClockPin, HIGH); // clock high
   usleep(CLOCK_DELAY);
   digitalWrite(bClockPin, LOW); // clock to low

//   pinMode(bDataPin, GPIO_OUT);
   
   return bAck;
} /* tm1637GetAck() */
//
// Write a unsigned char to the controller
//
static void tm1637WriteByte(unsigned char b)
{
unsigned char i;

   for (i=0; i<8; i++)
   {
      digitalWrite(bClockPin, LOW); // clock low
      if (b & 1) // LSB to MSB
         digitalWrite(bDataPin, HIGH);
      else
         digitalWrite(bDataPin, LOW);
      usleep(CLOCK_DELAY);
      digitalWrite(bClockPin, HIGH); // clock high
      usleep(CLOCK_DELAY);
      b >>= 1;
   }
} /* tm1637Writeunsigned char() */
//
// Write a sequence of unsigned chars to the controller
//
static void tm1637Write(unsigned char *pData, unsigned char bLen)
{
unsigned char b, bAck;
   bAck = 1;
   tm1637Start();
   for (b=0; b<bLen; b++)
   {
      tm1637WriteByte(pData[b]);
      bAck &= tm1637GetAck();
   }
   tm1637Stop();
} /* tm1637Write() */
//
// Set brightness (0-8)
//
void tm1637SetBrightness(unsigned char b)
{
unsigned char bControl;
   if (b == 0) // display off
      bControl = 0x80; // display off
   else
   {
      if (b > 8) b = 8;
      bControl = 0x88 | (b - 1);
   }
   tm1637Write(&bControl, 1);
} /* tm1637SetBrightness() */
//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1637ShowDigits(char *pString)
{
unsigned char b, bTemp[16]; // commands and data to transmit
unsigned char i, j;
   
   j = 0;
   bTemp[0] = 0x40; // memory write command (auto increment mode)
   tm1637Write(bTemp, 1);

   bTemp[j++] = 0xc0; // set display address to first digit command
   for (i=0; i<5; i++)
   {
      if (i == 2) // position of the colon
      {
         if (pString[i] == ':') // turn on correct bit
            bTemp[2] |= 0x80; // second digit high bit controls colon LEDs
      }
      else
      {
         b = 0;
         if (pString[i] >= '0' && pString[i] <= '9')
         {
            b = cDigit2Seg[pString[i] & 0xf]; // segment data
         }
         bTemp[j++] = b;
      }
   }
   tm1637Write(bTemp, j); // send to the display
}