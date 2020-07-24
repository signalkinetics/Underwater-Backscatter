// Underwater Acoustic Backscatter MCU
//
// Goal: Receive and decode a known query. Upon successful
//       decoding, backscatter signal by toggling between
//       reflective and non-reflective state
//
// Communication: FM0 (varying length of reflection followed by
//                     no a period of no reflection)
//                bit 1: long reflection (2*BIT_PERIOD)
//                bit 0: short reflection (BIT_PERIOD)
//
// Pin Layout
// Pin0: Controls the transistors to control
//       reflective/non-reflective state
// Pin2: Rectified singal input whose rising edge is
//       detected every clock cycle
// Pin3: Toggling at every detected rising edge (debug)
// Pin6: Swtich on the pull-down resistor to control
//       the receiving/transmitting mode

#include <msp430.h>
#include <stdint.h>

#define QUERY_LEN 9
#define PREAMBLE_LEN 20
#define TX_LEN 32
#define HALF_CYCLE 25
#define BIT_PERIOD 500
#define QUERY_FLIP_PERIOD 3000

uint16_t bit_start = 0, gap=1;
uint16_t now, bit, i;
uint8_t rx_fifo = 0, offset = 0, query = 0b10110100, mask=0xFF;
uint32_t tx_byte;
uint32_t preamble1 = 0b10110111010010001010;
uint32_t training = 0b00000000000000000000;
uint32_t data_byte = 0b10110100101101001011010010110100;
uint32_t data_byte2 = 0b10101010101010101010101010101010;
uint32_t data_byte3 = 0b11110000110010100000111100110101;
uint32_t bit_count;
uint8_t temp, state_AB, clk, state;
int cycle;

void printf(char *, ...);
void RX_Ready (void);
void TX_Byte(void);
void Preamble_Byte (void);

inline static void Clock_init(void);
//Sets up two clocks; one normal and another low-frequency low-power clock in order to use them 
//in the different states of the MCU

int main(void)
{
     WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT
     Clock_init();                                // Initialize the clocks

    // Configure GPIO
    P1DIR |= BIT0 + BIT4 + BIT3; // BIT3                 // Set pins to outputs
    P1OUT &= ~(BIT0 + BIT3);                             // piezo and load are connected
    P1OUT |=  BIT4; //BIT3                        // debug and ready to receive
    P1SEL = BIT2;                                 // Setup for clock interrupts on pin2

    TACTL = TASSEL_1 | MC_2 | TACLR;              // Sets up timer A.
												  //ACLK, continuous mode, clears timer A before starting, enable interrupt
	
												  
												  
												  
												  
	// Enter LPM3, enable interrupts

    cycle = 100;

    bit_count = PREAMBLE_LEN;
    // Send Preamble
    while (bit_count > 0) {

		for (i=0; i<cycle; i++) {};//Delays according to the cycle (in this case is 100)


		//In this piece of code you send a series of bits by toggling the device between the reflective/non-reflective states
		//You do this in a loop that last for as many bits as the preamble has.
		//BIT0 is the one in charge of toggling the device between the two modes
		//Most of the other variables are used for properly sending the desired preamble
        bit = (bit & 0) + clk & (preamble1 >> bit_count) & BIT0;
        state_AB = (((clk^(state_AB & BIT0)) & BIT0) << 1) + ((((~bit) & state_AB&BIT0)|((~clk)& state_AB&BIT0)|(clk&bit&(~(state_AB&BIT0))))&BIT0);
        temp = ((clk & BIT0) * ((state_AB & BIT1) >> 1) + ((~clk) & BIT0) * (state_AB & BIT0));
        P1OUT = (P1OUT & ~BIT0) + (temp & BIT0);
        clk = (~clk) & BIT0;
        bit_count -= clk;
    }

	//Sending another piece of information
    bit_count =200;
    while (bit_count > 0) {

        for (i = 0; i < cycle; i++) {};// Delay used according to the cycle(in this case is 100)
        bit = 1; 


        state_AB = (((clk^(state_AB & BIT0)) & BIT0) << 1) + ((((~bit) & state_AB&BIT0)|((~clk)& state_AB&BIT0)|(clk&bit&(~(state_AB&BIT0))))&BIT0);



        temp = ((clk & BIT0) * ((state_AB & BIT1) >> 1) + ((~clk) & BIT0) * (state_AB & BIT0));
        P1OUT = (P1OUT & ~BIT0) + (temp & BIT0);
        clk = (~clk) & BIT0;
        bit_count -= clk;

    }
    P1OUT &= ~BIT0;

//     sleep

}

// Set the MCU into receiving mode
void RX_Ready(void) {
    P1OUT &= ~(BIT0 + BIT1);//Puts BIT0 = 0 to toggle into receiving mode
    rx_fifo = 0;
    bit_count = 8;
    // Configure Timer_A
    TACCTL1 = SCS + CM_1 + CAP + CCIE + CCIS_0;
	//Timer A1
	//SCS = Capture Synchronize
	//CM_1 = Capture mode: 1- pos. edge
	//CAP = Capture mode: 1
	//CCIE = Capture/Compare Interrupt enable
	//CCIS_0 = Capture input select: 0 - CCIxA
}

// Set the MCU to transmitting mode
void TX_Byte(void) {
    state_AB = 0b00;
    clk = 1;
    bit_count = TX_LEN; //data    32
    tx_byte = data_byte2;//This is the transmitted data
    // Set timer interrupts
    TACCR1 = TAR + HALF_CYCLE;
    // Reconfigure Timer_A
    TACCTL1 = CM_0 + CCIE;
	//This configuration makes it not being in capture mode
	//but it has enabled interrupts in case new information is arriving
}

void Preamble_Byte(void) {
	//This function is used for transmitting the preamble
    state = 1;
    state_AB = 0b00;
    clk = 1;
    bit_count = PREAMBLE_LEN; // preamble
    tx_byte = preamble1; //training;
    // Set timer interrupts
    TACCR1 = TAR + HALF_CYCLE;
    // Reconfigure Timer_A
    TACCTL1 = CM_0 + CCIE;
}


// Timer0_A1 Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA0IV,TA0IV_TAIFG))
    {
        case TA0IV_NONE:
            break;//In case of no interrupt pending just continue
        case TA0IV_TACCR1:
            // RX mode
            if(TACCTL1 & CAP) {
                now = TACCR1; // timestamp
                // we start measuring the duration of amplitude flip on the first
                // rising edge. Hence, we let the bit_count start at 9 instead of 8
                if (bit_count < 8) {
                    if ((TACCTL1 & BIT1) == 0) {
                        // Determine how long for a amplitude flip to determine bit received
                        bit = (now - bit_start) > QUERY_FLIP_PERIOD ? 1 : 0;
                        rx_fifo |= bit << bit_count;
                    } else { // There was a second capture unfortunately
                        TACCTL1 &= ~(BIT1);
                    }
                }
                // All the bits have been received
                if (bit_count == 0) {

                    // Compare the bits first
                    offset = 0;
                    while ((offset < QUERY_LEN) \
                            && ((rx_fifo & (mask >> offset)) != (query >> offset))) {
                        offset++;
                    }
                    // successful
                    if (offset == 0) {
                        P1OUT |= BIT3;
                        TACCTL1 &= ~CCIE;                  // All bits RXed, disable interrupts from capture
                        __bic_SR_register_on_exit(LPM3_bits); // CLear LPM3its to start transmitting
                    } else {
                    // failed
                        RX_Ready();
                    }
                }
                bit_count--;
                bit_start = now;
            // TX mode
            } else {
                TACCR1 += HALF_CYCLE;
                if (bit_count == 0 && clk == 1) {
                    state += 1;
                    if (state == 1) {
                        bit_count = PREAMBLE_LEN;
                        tx_byte = preamble1;
                    } else if(state == 2) {
                        bit_count = 1000; //TX_LEN;
                        tx_byte = data_byte2;
                    } else if (state > 2){
                        TACCTL1 &= ~CCIE;
                        __bic_SR_register_on_exit(LPM3_bits);
                    }
                }
                bit_count -= clk;
                if (state == 0) {
                    bit = 0;
                } else if (state == 1) {
                    bit = (bit & 0) + clk & (tx_byte >> bit_count) & BIT0;
                } else if (state == 2) {
                    bit = (bit ^ clk) & BIT0;
                }
                state_AB = (((clk^(state_AB & BIT0)) & BIT0) << 1) + ((((~bit) & state_AB&BIT0)|((~clk)& state_AB&BIT0)|(clk&bit&(~(state_AB&BIT0))))&BIT0);
                temp = ((clk & BIT0) * ((state_AB & BIT1) >> 1) + ((~clk) & BIT0) * (state_AB & BIT0));
                P1OUT = (P1OUT & ~BIT0) + (temp & BIT0);
                clk = (~clk) & BIT0;


                }
            TACCTL1 &= ~(BIT0);
            break;                               // CCR1 not used
        case TA0IV_TACCR2:
            break;                               // CCR2 not used
        case TA0IV_TAIFG:
            break;
        default:
            break;
    }

}

inline static void Clock_init(void)
{
    BCSCTL1 = CALBC1_1MHZ;              // Set Digitally Controlled Oscillator (DCO) to default of 1Mhz
    DCOCTL = CALDCO_1MHZ;			    // Set the DCO Register to 1 Mhz  (step & modulation)

    BCSCTL3 = LFXT1S_2;                // LFXT1 = VLO (Very Low-Power Low-Frequency Oscillator). Clock that uses little energy for functioning and no energy when off.
    IFG1 &= ~(OFIFG);                   // Clear OSCFault flag (Interrupt Flag Register)
}
