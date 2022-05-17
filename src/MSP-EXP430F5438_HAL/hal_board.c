/* ---Hardware includes--- */
#include "msp430.h"
#include "hal_MSP-430F5438-MoteIST.h"

/* ---RTOS includes--- */
#include "FreeRTOS.h"
#include "task.h"
 
/*-----------------------------------------------------------------------------------------*/

/* --- Function Prototypes --- */

/*-----------------------------------------------------------------------------------------*/

void hal_i2c_init_slave(unsigned char my_addr){
	UCB1CTL1 |= UCSWRST;                    // Enable SW reset
	P3SEL |= BIT7;                          // P3.7 - UCB1SDA
	P5SEL |= BIT4;			    			// P5.4 - UCB1SCL
	UCB1CTL0 = UCMODE_3 + UCSYNC;           // I2C Slave, synchronous mode
	UCB1I2COA = 0x40;                      	// Own Address 
	UCB1CTL1 &= ~UCSWRST;                   // Clear SW reset, resume operation
	UCB1IE |= UCSTPIE + UCSTTIE + UCRXIE;   // Enable STT, STP & RX interrupt

}


void hal_setup_leds(void){
	LED_PORT_SEL &= ~(LED_R | LED_O | LED_G);
	LED_PORT_DIR |= (LED_R | LED_O | LED_G);
	LED_PORT_OUT |= (LED_R | LED_O | LED_G);	

}

void hal_toggle_leds(void){
	//	LED_PORT_OUT ^= LED_R; // OS_TICK already blink the GREEN (LED_1) led... 
	LED_PORT_OUT ^= LED_2;
	LED_PORT_OUT ^= LED_3;
}

void hal_toggle_led(unsigned char led){
	LED_PORT_OUT ^= led;
}


/**********************************************************************//**
 * @brief  Outputs clock to testpoints
 *
 * @param  none
 *
 * @return none
 *************************************************************************/
void halBoardOutputSystemClock(void){
	CLK_PORT_DIR |= 0x07;
	CLK_PORT_SEL |= 0x07;
}

/**********************************************************************//**
 * @brief  Stops the output of ACLK, MCLK, SMCLK on P11.0, P11.1, and P11.2.
 *
 * @param  none
 *
 * @return none
 *************************************************************************/
void halBoardStopOutputSystemClock(void){
	CLK_PORT_OUT &= ~0x07;
	CLK_PORT_DIR |= 0x07;	
	CLK_PORT_SEL &= ~0x07;
}


void halBoardInit(void){

	/* Disable the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;

	/* 
	 * NOTES:
	 * slaa395 recommends output direction by default...
	 *
	 * Here we are using the pull-down registors...
	 */

	PASEL = 0;
	PADIR = 0;
	PAREN = 0xFFFF;
	PAOUT = 0;

	PBSEL = 0; //P5SEL |= 0x0C;              // Port select XT2
	PBDIR = 0;
	PBREN = 0xFFFF;	//PBREN = 0xFFF3
	PBOUT = 0;

	PCSEL = 0;
	PCDIR = 0;
	PCREN = 0xFFFF;
	PCOUT = 0;

	PDSEL = 0x0003; // P7SEL0,1 -> XIN e XOUT (cristal pins)
	PDDIR = 0;
	PDREN = 0xFFFC;
	PDOUT = 0;

	PESEL = 0;
	PEDIR = 0;
	PEREN = 0xFFFF;
	PEOUT = 0;

	PFSEL = 0; // only 8 bit port..
	PFDIR = 0;
	PFREN = 0xFF;
	PFOUT = 0;

}

/**********************************************************************//**
 * @brief  Set function for MCLK frequency.
 *
 *
 * @return none
 *************************************************************************/
void hal430SetSystemClock(unsigned long req_clock_rate, unsigned long ref_clock_rate){
	/* Convert a Hz value to a KHz value, as required
	 *  by the Init_FLL_Settle() function. */
	unsigned long ulCPU_Clock_KHz = req_clock_rate / 1000UL;

	//Make sure we aren't overclocking
	if(ulCPU_Clock_KHz > 25000L)
	{
		ulCPU_Clock_KHz = 25000L;
	}

	//Set VCore to a level sufficient for the requested clock speed.
	if(ulCPU_Clock_KHz <= 8000L)
	{
		SetVCore(PMMCOREV_0);
	}
	else if(ulCPU_Clock_KHz <= 12000L)
	{
		SetVCore(PMMCOREV_1);
	}
	else if(ulCPU_Clock_KHz <= 20000L)
	{
		SetVCore(PMMCOREV_2);
	}
	else
	{
		SetVCore(PMMCOREV_3);
	}

	//Set the DCO
	Init_FLL_Settle( ( unsigned short )ulCPU_Clock_KHz, req_clock_rate / ref_clock_rate );
}
