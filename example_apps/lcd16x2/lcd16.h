/*
 * lcd16.h
 *
 *  Created on: Dec 1, 2011
 *      Author: Gaurav www.circuitvalley.com
 */

#ifndef LCD16_H_
#define LCD16_H_


#include <msp430x16x.h>
#define  EN BIT6
#define  RS BIT7

void waitlcd(unsigned int x);

void lcdinit(void);
void integerToLcd(int integer );
void lcdData(unsigned char l);
void prints(char *s);
void gotoXy(unsigned char  x,unsigned char y);
#endif /* LCD16_H_ */
