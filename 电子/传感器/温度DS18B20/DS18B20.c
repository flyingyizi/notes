/********************************
FILE NAME:        DS18B20.c
CHIP TYPE:        ATMEGA16
CLOCK FREQUENCY:  8MHZ
IDE:              VSMStudio
COMPILER:         AVR-GCC
TIME:             September 2010
********************************/

#include <avr/io.h>
#include <util/delay.h>

#define uchar unsigned char
#define uint unsigned int
#define BUS PORTC

// Low level port/pin definitions
#define sbit(x,PORT) (PORT) |= (1<<x)
#define cbit(x,PORT) (PORT) &= ~(1<<x)
#define pin(x,PIN) (PIN) & (1<<x)

// Pins definition
#define s_digit1 sbit(5,PORTC) 
#define c_digit1 cbit(5,PORTC)
#define s_digit2 sbit(4,PORTC) 
#define c_digit2 cbit(4,PORTC)
#define out    PORTC
#define DQ_IN  DDRA&=~(1<<7)
#define DQ_OUT DDRA|= _BV(DDA7) // (1<<7)
#define S_DQ   sbit(7,PORTA)
#define C_DQ   cbit(7,PORTA)
#define DQ     pin(7,PINA) 

// Function Prototypes
void init_ds18b20(void);
uchar readbyte(void);
void writecommand(uchar);
uchar readtemp(void);
uchar a, b, tt;

// Main program
int main(void)
 { uchar i=0, temp;
   // Initialize Stack Pointer
   SPL=0x54;
   SPH=0x04;
   // Configure port pins
   DDRC = 0xff;
   DDRA = 0xff;
   while(1)
    { temp = readtemp();
      for(i=0; i<10; i++)     // 10 measures
       { // output the units
         out = (temp/10) & 0x0f;
         s_digit1;
         c_digit2;
         _delay_ms(5);  
         // output the tens
         out = (temp%10) & 0x0f;
         c_digit1;
         s_digit2;
         _delay_ms(5);  
       }
    }
 }

// Start transaction with 1-wire line. 
void init_ds18b20(void)
 { DQ_OUT; 
   C_DQ ;    
   _delay_us(600); 
   S_DQ;    
   _delay_us(50);  
   DQ_IN; 
   while(DQ);
   _delay_us(240); 
   DQ_OUT;   
   S_DQ;    
   _delay_us(300); 
 }

// Read a byte from the sensor
uchar readbyte(void)
 { uchar i = 0,data = 0;
   DQ_OUT; 
   for (i=0; i<8; i++)
    { C_DQ ;
      data >>= 1;
      _delay_us(3);
      S_DQ;
      DQ_IN; 
      _delay_us(12);
      if(DQ)
         data |= 0x80;
      DQ_OUT;
      S_DQ;
      _delay_us(45);
      _delay_us(5);
    }
   return(data);
 }

// Write a command to the sensor
void writecommand(uchar data)
 { uchar  i;
   for(i=0; i<8; i++)
    { C_DQ;
      _delay_us(15);    
      if(data & 0x01)
         S_DQ;
      else 
         C_DQ;
      _delay_us(45);
      data >>= 1;
      S_DQ;
      _delay_us(2);
    }
 }

// Read value from the sensor
uchar readtemp(void)
 { uint t;
   init_ds18b20();
   // Convert
   writecommand(0xCC); 
   writecommand(0x44);
   init_ds18b20();
   // Read Scratch memory area
   writecommand(0xCC); 
   writecommand(0xBE); 
   a = readbyte();
   b = readbyte();
   t = b;
   t <<= 8;
   t = t|a;
   tt = t*0.0625;
	return(tt);
 }
