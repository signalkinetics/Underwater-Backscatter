/* limboG2211: LPM test to see how lpm's affect I/O behavior.  Designed with
 * a speaker attached to P1.0 and P1.2 in mind, but can be simulated by using
 * the two LEDs on P1.0 and P1.6 with a frequency low enough to be able to see
 * the change.  To switch to LED visualization, change the definition of SOUT
 * to read #define SOUT   BIT6 and drop the frequency by increasing TACCR0.
 */

#include <msp430g2211.h>

#define SPKR   BIT0
#define SOUT   BIT2

void main(void) {
	
	WDTCTL = WDTPW + WDTHOLD;	// Disable watchdog
	
	P1OUT = SPKR;			    // Could easily start with SPKR off, but this
	                            // initial output makes it easier to switch to
	                            // LED visualization.
	P1DIR = SPKR + SOUT;        // The other end of the speaker is held at
							    // ground.  Volume can be increased by
							    // switching SOUT alternately with SPKR.
	
	BCSCTL1 = CALBC1_1MHZ;		// Running at 1 MHz
	DCOCTL = CALDCO_1MHZ;

	TACCR0 = 144;           // With the Timer using SMCLK div 8 (125 kHz), this
							// value gives a frequency of 125000/(TACCR0+1) Hz.
							// For TACCR0 = 144, that's 862 Hz.
							
	TACCTL0 = CCIE;         // Enable interrupts for CCR0.
	TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR;  // SMCLK, div 8, up mode,
											 // clear timer.
	
	_BIS_SR(LPM0_bits + GIE);	// Enter LPM0 and enable interrupts
	
} // main


/*  Interrupt Service Routines  */
#pragma vector = TIMERA0_VECTOR
__interrupt void CCR0_ISR(void) {
		P1OUT ^= SPKR + SOUT;	// Alternate output pins.
} // CCR0_ISR
