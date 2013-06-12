/*
 * main.c www.circuitvalley.com
 */

#include <msp430x16x.h>
#include "lcd16.h"
void main(void) {
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR = 0xFF;
	P1OUT = 0x00;
	lcdinit();
prints("MSP430 16x2  LCD");

		gotoXy(0,1);
		prints(" circuitvalley");
	//    prints("Integer = ");
	//	integerToLcd(3245);
	while(1){

	}
}
