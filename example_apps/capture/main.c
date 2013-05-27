#include  <msp430x241x.h>
#include <signal.h>

unsigned int new_cap=0;
unsigned int old_cap=0;
unsigned int cap_diff=0;

unsigned int diff_array[16];                // RAM array for differences
unsigned int capture_array[16];             // RAM array for captures
unsigned char id=0;
unsigned char count = 0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  
  P1DIR = 0x01;                             // Set P1.0 out,1.1 input dir
  P1SEL = 0x02;                             // Set P1.1 to TA0
  CCTL0 = CM_1 + SCS + CCIS_0 + CAP + CCIE; // Rising edge + CCI0A (P1.1)
                                            // + Capture Mode + Interrupt
  TACTL = TASSEL_2 + MC_2;                  // SMCLK + Continuous Mode

  _BIS_SR(LPM0_bits + GIE);                 // LPM0 + Enable global ints
  return 0;
}

interrupt(TIMERA0_VECTOR) TimerA0(void)
{
   new_cap = TACCR0;
   cap_diff = new_cap - old_cap;

   diff_array[id] = cap_diff;            // record difference to RAM array
   capture_array[id++] = new_cap;
   if (id == 16)
   {
     id = 0;
   }
   old_cap = new_cap;                       // store this capture value
   count ++;
   if (count == 32)
   {
     count = 0;
   }

}
