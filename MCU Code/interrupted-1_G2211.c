/* interrupted-1_G2211: This code is a restructured version of the dcodemo
 * code done previously that makes use of interrupts.  A push-button on P1.3
 * issues a cycle of the DCO frequency.
 */

#include <msp430g2211.h>

#define LED1  BIT0


/*  Global Variables  */
char i=0;                   // char values are used to preserve memory
char bcs_vals[3] = {7,9,2};
char dco_vals[3] = {3,5,6};

/*  Function Declarations  */
void delay(void);

void main(void) {
	
	WDTCTL = WDTPW + WDTHOLD;    // disable watchdog
	
	P1OUT = 0;
	P1DIR = LED1;    // P1.0 out to LED, P1.3 remains input for BTN1
	P1IES |= BIT3;   // high -> low is selected with IESx = 1.
	P1IFG &= ~BIT3;  // To prevent an immediate interrupt, clear the flag for
	                 // P1.3 before enabling the interrupt.
	P1IE |= BIT3;    // Enable interrupts for P1.3
	
	_enable_interrupt();
	
	for (;;) {           // Note the change in method; rather than flashing 5
		P1OUT ^= LED1;   // times, this program flashes the LED continuously.
		delay();
	}
} // main

void delay(void) {
	unsigned int n;
	for (n=0; n<60000; n++);
} // delay

/*  Interrupt Service Routines  */
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {
	switch(P1IFG & BIT3) {
		case BIT3:
			P1IFG &= ~BIT3;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];
			if (++i == 3)
				i = 0;
			return;
		default:
			P1IFG = 0;  // probably unnecessary, but if another flag occurs
			            // in P1, this will clear it.  No error handling is
			            // provided this way, though.
			return;
	}
} // P1_ISR
