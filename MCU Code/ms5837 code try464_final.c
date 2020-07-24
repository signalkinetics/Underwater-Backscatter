//******************************************************************************
//  MSP430G2553 USCI_B0 I2C connection to ms5837-30BA 
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.0MHz
//
//
//                                /|\  /|\
//                  MS5837        10k  10k   MSP430G2553
//                   slave         |    |      master
//             -----------------   |    |  -----------------
//            |       SDA       |<-|---+->|P1.7/UCB0SDA  
//            |                 |  |      |                 
//            |                 |  |      |             
//            |       SCLK      |<-+----->|P1.6/UCB0SCL     
//            |                 |         |                 
//
//******************************************************************************
#include <msp430.h>
#include <stdint.h>

#define NUM_BYTES_TX 1                  		
// FUNCTION RETURN VALUES
#define RET_SUCCESS		0
#define RET_FAIL			1
#define RET_FAIL_CALIB		4
#define RET_TIMEOUT		5

volatile int RXByteCtr, i;    	
volatile unsigned char RxBuffer[22];  			// Allocate 22 bytes of RAM
volatile int Rx_idx=0, Tx_idx=0;
volatile char TXByteCtr, RX = 0;
volatile char MSData[13]={0x1E,0xA0,0xA2,0xA4,0xA6,0xA8,0xAA,0xAC,0x4A,0,0x5A,0,0};
volatile char ret, receivedCrc, calculatedCrc;
volatile unsigned short buff[8];
volatile int NUM_BYTES_RX = 2;				// Received bytes = 2 for prom
volatile unsigned short sensorC[7];
volatile signed long D1=0, D2=0, dT, TEMP, PRESSURE; 
volatile signed long long SENS, pressure, temp, OFF;

void Setup_TX(void);
void Setup_TX1(void);
void Setup_RX(void);
void Transmit(void);
void Receive(void);
char unlockI2Cline(unsigned short maxLoops);
char isMS5837Available();
unsigned char crc4(volatile unsigned short n_prom[]);  


int main(void)
{
 	WDTCTL = WDTPW + WDTHOLD;              		// Stop WDT

	// Initialize P2 for ms5837 status
	P2DIR |= BIT0;                   				// P2.0 output
	P2OUT &= ~BIT0;							// Set P2.0 to low

  	P1SEL |= BIT6 + BIT7;                 		// Assign I2C pins to USCI_B0
  	P1SEL2|= BIT6 + BIT7;                  		// Assign I2C pins to USCI_B0
    
  	//Transmit process
  	Setup_TX(); 
  	Transmit(); 
	__delay_cycles(100000);  				// 100ms,Make sure reset done
  	while (UCB0CTL1 & UCTXSTP);

	for (i=0;i<7;i++)
	{	
  		Tx_idx++;
		Setup_TX1(); 
  		Transmit(); 
  		while (UCB0CTL1 & UCTXSTP);    			// Ensure stop condition got sent

  		//Receive process
  		Setup_RX();	
  		Receive();	
		__delay_cycles(10000);
  		while (UCB0CTL1 & UCTXSTP);     		// Ensure stop condition got sent
	}

	for (i=0;i<7;i++)
	{ 
		buff[i] = (RxBuffer[i*2]<<8) + RxBuffer[i*2+1]; 
		sensorC[i] = buff[i];
	}
	buff[7] = 0x0000;
	receivedCrc = ((buff[0] >> 8) >> 4) & 0x0F;	// Read received CRC
	calculatedCrc = crc4(buff);
	if(calculatedCrc != receivedCrc) ret = 33; 	// Compare with calculated CRC-4
	
	NUM_BYTES_RX = 3;
	for (i=0;i<2;i++)
	{	
  		Tx_idx++;
		Setup_TX1(); 
  		Transmit(); 
  		while (UCB0CTL1 & UCTXSTP);    		// Ensure stop condition got sent
		__delay_cycles(30000);		// ADC conversion > 18.08ms

		Tx_idx++;
		Setup_TX1(); 
  		Transmit(); 
  		while (UCB0CTL1 & UCTXSTP);    		// Ensure stop condition got sent

  		//Receive process
  		Setup_RX();	
  		Receive();	
		__delay_cycles(10000);
  		while (UCB0CTL1 & UCTXSTP);     	// Ensure stop condition got sent
	}
	
	__disable_interrupt();
	D1 = (signed long)RxBuffer[14]*256*256+ (signed long)RxBuffer[15]*256+ (signed long)RxBuffer[16];
	D2 = (signed long)RxBuffer[17]*256*256+ (signed long)RxBuffer[18]*256+ (signed long)RxBuffer[19];

	// Calculate compensated temperature and pressure
	dT = D2 - ((signed long)sensorC[5] * 256);
	OFF = ((signed long long)sensorC[2] * 65536) + (((signed long long)sensorC[4] * (signed long long) dT)/128);
	SENS = ((signed long long)sensorC[1] * 32768) + (((signed long long)sensorC[3] * (signed long long) dT)/256);
	temp = 2000 + ((signed long long)dT * (signed long long)sensorC[6])/8388608;
	TEMP = temp;
	pressure = (((signed long long)D1 * SENS / 2097152) - OFF) / 8192;
	PRESSURE = pressure;

	// If CRC correct AND 10degC < temperature < 30degC
	// AND 0.9bar < pressure < 2.1 bar (11m depth water)
	// toggle p2.0 at 1Hz for 5 times, otherwise p2.0 kept logic 0

	if ((receivedCrc == calculatedCrc) && (TEMP<3000 && TEMP>1000) && (PRESSURE<21000 && PRESSURE>9000)) 
	{	
		for (i=0;i<5;i++)
		{
			P2OUT |= BIT0 + BIT1;		// results correct
			__delay_cycles(500000);  		// p2.0 = 1 for 0.5s 
			P2OUT &= ~BIT0; P2OUT &= ~BIT1;	
        __delay_cycles(500000);  		// p2.0 = 0 for 0.5s
		}
	}	
	else P2OUT &= ~BIT0;			// ms5837 failed

	while(1){ __delay_cycles(100000);}		// put breakpoint here
}


unsigned char crc4(volatile unsigned short n_prom[])  
{
	int cnt; // simple counter
	unsigned int n_rem=0; 		// crc remainder
	unsigned char n_bit;
	n_prom[0]=((n_prom[0]) & 0x0FFF); 		// CRC byte is replaced by 0
	n_prom[7]=0;  				// Subsidiary value, set to 0
	for (cnt = 0; cnt < 16; cnt++) 		// operation is performed on bytes
	{ // choose LSB or MSB
		if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
		else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))  n_rem = (n_rem << 1) ^ 0x3000;
			else  n_rem = (n_rem << 1);
		}
	}
	n_rem= ((n_rem >> 12) & 0x000F); 		// final 4-bit remainder is CRC code
	return (n_rem ^ 0x00);
}


//-------------------------------------------------------------------------------
// The USCI_B0 data ISR is used to move received data from the I2C slave
// to the MSP430 memory. It is structured such that it can be used to receive
// any 2+ number of bytes by pre-loading RXByteCtr with the byte count.
//-------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  	if(RX == 1)                        		// Master Recieve?
	{
  		RXByteCtr--;                     		// Decrement RX byte counter
  		if (RXByteCtr)
  		{
    		RxBuffer[Rx_idx]= UCB0RXBUF;   		// Move RX data to address PRxData
			Rx_idx++;	
			if (RXByteCtr==1)	UCB0CTL1 |= UCTXSTP;              
  		}
  		else
  		{
   		//UCB0CTL1 |= UCTXSTP;              // No Repeated Start: stop condition
    		RxBuffer[Rx_idx]= UCB0RXBUF;       	// Move final RX data to PRxData
			Rx_idx++;
    		__bic_SR_register_on_exit(CPUOFF); 	// Exit LPM0
  		}
	}
  
  	else                                 		// Master Transmit
	{
  		if (TXByteCtr)                    		// Check TX byte counter
  		{
    		UCB0TXBUF = MSData[Tx_idx];     		// Load TX buffer
    		TXByteCtr--;                 			// Decrement TX byte counter
  		}
  		else
  		{
			IFG2 &= ~UCB0TXIFG;			// Clear USCI_B0 TX int flag
			IE2 &= ~UCB0TXIE;			// Disable Tx Interrupt
			UCB0CTL1 |= UCTXSTP;			// Send a STOP right after the command
    		__bic_SR_register_on_exit(CPUOFF); 	// Exit LPM0
  		} 
	}
}


void Setup_TX(void){
  	__disable_interrupt();
  	RX = 0;
  	IE2 &= ~UCB0RXIE;  
  	while (UCB0CTL1 & UCTXSTP);      			// Ensure stop condition got sent
  	UCB0CTL1 |= UCSWRST;           			// Enable SW reset
	ret = unlockI2Cline(5);
  	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;  	// I2C Master, synchronous mode
  	UCB0CTL1 = UCSSEL_2 + UCSWRST;    			// Use SMCLK, keep SW reset
  	UCB0BR0 = 12;                 			// fSCL = SMCLK/12 = ~100kHz
  	UCB0BR1 = 0;
  	UCB0I2CSA = 0x76;          			// Slave Address is 076h
  	UCB0CTL1 &= ~UCSWRST;       			// Clear SW reset, resume operation
}


void Setup_TX1(void){
  __disable_interrupt();
  RX = 0;
  IE2 &= ~UCB0RXIE;  
  while (UCB0CTL1 & UCTXSTP);           	// Ensure stop condition got sent
  UCB0CTL1 |= UCSWRST;                  	// Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;  	// I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;        	// Use SMCLK, keep SW reset
  UCB0BR0 = 12;                         	// fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = 0x76;                   		// Slave Address is 076h
  UCB0CTL1 &= ~UCSWRST;              		// Clear SW reset, resume operation
}


void Setup_RX(void){
  __disable_interrupt();
  RX = 1;
  IE2 &= ~UCB0TXIE;  
  UCB0CTL1 |= UCSWRST;                   	// Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;  	// I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;        	// Use SMCLK, keep SW reset
  UCB0BR0 = 12;                         	// fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = 0x76;                    		// Slave Address is 076h
  UCB0CTL1 &= ~UCSWRST;               		// Clear SW reset, resume operation
}


void Transmit(void){
 	//Tx_idx = 0;                      		// TX array start address
 	TXByteCtr = NUM_BYTES_TX;          		// Load TX byte counter
 	while (UCB0CTL1 & UCTXSTP);         		// Ensure stop condition got sent
	IE2 &= ~UCB0RXIE;                    		// Disable RX interrupt
	IE2 |= UCB0TXIE;       			// Enable TX interrupt
 	UCB0CTL1 |= UCTR + UCTXSTT;       			// I2C TX, start condition
 	__bis_SR_register(CPUOFF + GIE);  			// Enter LPM0 w/ interrupts
}


void Receive(void){
 	//Rx_idx = 0;    					// Start of RX buffer
 	RXByteCtr = NUM_BYTES_RX;        		// Load RX byte counter
 	while (UCB0CTL1 & UCTXSTP);     		// Ensure stop condition got sent
	IE2 &= ~UCB0TXIE;          		// Disnable TX interrupt
	IE2 |= UCB0RXIE;             		// Enable RX interrupt
	UCB0CTL1 &= ~UCTR;
 	UCB0CTL1 |= UCTXSTT;          		// I2C start condition
 	__bis_SR_register(CPUOFF + GIE);  		// Enter LPM0 w/ interrupts
}


// Toggle SCL line untill it unlocks
char unlockI2Cline(unsigned short maxLoops)
{
	if(isMS5837Available())
		return 55;	//RET_SUCCESS;

	// Loop for a max of maxLoops
	do
	{
		// First, de-activate I2C
		UCB0CTL1|= UCSWRST;

		// Return the pins to GPIO and toggle SCL signal once
		// Re-select GPIO
		P1SEL &= ~(BIT6 | BIT7);
		P1SEL2 &= ~(BIT6 | BIT7);
		// SDA low
		P1OUT |= BIT7;
		// SCL High
		P1OUT |= BIT6;
		// Both outputs
		P1DIR |= BIT6 | BIT7;
		// SCL low (toggled)
		P1OUT &= ~BIT6;
		// SCL High
		P1OUT |= BIT6;
		// SCL low (toggled)
		P1OUT &= ~BIT6;
		// SCL High
		P1OUT |= BIT6;

		// Return to I2C
		P1SEL |= BIT6 + BIT7;		// Assign I2C pins to USCI_B0
    	P1SEL2|= BIT6 + BIT7;		// Assign I2C pins to USCI_B0
		UCB0CTL1 &= ~UCSWRST; 	// Clear SW reset, resume operation

		maxLoops--;
	}while(isMS5837Available()==0 && maxLoops > 0);

	// if the line is still busy, return a timeout
	if(isMS5837Available()==0)
		return 77;	//RET_TIMEOUT;
	return 66;	//RET_SUCCESS;
}


char isMS5837Available()
{
	return !(UCB0STAT & UCBBUSY || IE2 & UCB0TXIE || IE2 & UCB0RXIE || UCB0CTL1 & UCTXSTP);
}

