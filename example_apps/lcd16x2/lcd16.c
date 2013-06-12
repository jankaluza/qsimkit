/*
 * lcd16.c
 *
 *  Created on: Dec 1, 2011
 *      Author: Gaurav www.circuitvalley.com
 */
#include "lcd16.h"

void lcdcmd(unsigned char Data)
{

P1OUT &= ~RS; //because sending command
P1OUT  &=~EN;

P1OUT &= 0xF0;
P1OUT |= ((Data >> 4) & 0x0F);
P1OUT  |=EN;
waitlcd(2);
P1OUT  &=~EN;

P1OUT &= 0xF0;
P1OUT  |= (Data & 0x0F);
P1OUT  |=EN;
waitlcd(2);
P1OUT  &=~EN;


}
void lcdData(unsigned char l)
{
P1OUT |=RS;  //because sending data
P1OUT  &=~EN;
P1OUT &= 0xF0;
P1OUT |=((l >> 4) & 0x0F);

P1OUT  |=EN;
waitlcd(2);
P1OUT  &=~EN;
P1OUT &=  0xF0;
P1OUT  |= (l & 0x0F);
P1OUT  |=EN;
waitlcd(2);
P1OUT  &=~EN;



}

void lcdinit(void)
{
P1OUT &=~RS;
P1OUT  &=~EN;
P1OUT |= 0x3;
waitlcd(40);
P1OUT  |=EN;
P1OUT  &=~EN;
waitlcd(5);
P1OUT  |=EN;
P1OUT  &=~EN;
waitlcd(5);
P1OUT  |=EN;
P1OUT  &=~EN;
waitlcd(2);

P1OUT &= 0xF2;
P1OUT  |=EN;
P1OUT  &=~EN;
lcdcmd(0x28);   //set data length 4 bit 2 line
waitlcd(250);
lcdcmd(0x0E);   // set display on cursor on blink on
waitlcd(250);
lcdcmd(0x01); // clear lcd
waitlcd(250);
lcdcmd(0x06);  // cursor shift direction
waitlcd(250);
lcdcmd(0x80);  //set ram address
waitlcd(250);
}

void waitlcd(volatile unsigned int x)
{
volatile unsigned int i;
for (x ;x>1;x--)
{
for (i=0;i<=110;i++);
}
}

void prints(char *s)
  {

    while (*s)
      {
	 lcdData(*s);
	 s++;
      }
  }

void gotoXy(unsigned char  x,unsigned char y)
{
 if(x<40)
 {
  if(y) x |= 0x40;
  x |=0x80;
  lcdcmd(x);
  }

}
void integerToLcd(int integer )
{

unsigned char thousands,hundreds,tens,ones;
thousands = integer / 1000;

    lcdData(thousands + 0x30);

	 hundreds = ((integer - thousands*1000)-1) / 100;

	lcdData( hundreds + 0x30);
tens=(integer%100)/10;

	lcdData( tens + 0x30);
	ones=integer%10;

	lcdData( ones + 0x30);
}
