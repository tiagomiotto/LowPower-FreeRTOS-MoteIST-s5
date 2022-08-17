/*
 * uart.c
 *
 *  Created on: May 17, 2015
 *      Author: Diogo Guerra
 */

 /* ---Hardware includes--- */
#include "hal_MSP-430F5438-MoteIST.h"

/* ---Scheduler includes--- */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

/* ---Other includes--- */
#include "uart.h"


/***********************************************************************/


/*VARIABLE DECLARATIONS*/
#if UART_A0 == 1


#endif
#if UART_A1 == 1

	static unsigned char * pcUARTTXBuffer;
	static unsigned char * pcUARTRXBuffer;
	static unsigned int 	uiTXNumBytes;
	static unsigned int 	uiRXNumBytes;

#endif
#if UART_A2 == 1


#endif



/***********************************************************************/
/***********************************************************************/
/***********************************************************************/



//SEE SITE FOR BITRATE
//mspgcc.sourceforge.net/baudrate.html
//mspgcc.sourceforge.net/cgi-bin/msp-uart.pl?clock=25034752&baud=921600&submit=calculate
#if UART_A0 == 1

	void vHalSetupMoteUSB(void){

		P3SEL |= 0x30;             	// P3.4,5 = USCI_A0 TXD/RXD
		UCA0CTL1 |= UCSWRST;      	// **Put state machine in reset**
		UCA0CTL1 |= UCSSEL_2;     	// SMCLK //_1 ACLK
		UCA0BR0 = 0x2c;				// 25MHz 115200baud (see http://mspgcc.sourceforge.net/baudrate.html)
		UCA0BR1 = 0x0A;				// 25MHz 115200baud
		UCA0MCTL = 0x00;			// Modulation UCBRSx=1, UCBRFx=0	
		UCA0CTL1 &= ~UCSWRST;     	// **Initialize USCI state machine**
	}
	
#endif /*UART_A0*/
#if UART_A1 == 1

	void vInstallPeripheralUART(void){
		P5SEL |= (BIT7 | BIT6);
		UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
		UCA1CTL1 |= UCSSEL_2;                     // SMCLK
		UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
		UCA1BR1 = 0;                              // 1MHz 115200
		UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
		UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
		//UCA2IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt
	}

#endif /*UART_A1*/
#if UART_A2 == 1

	void vHalSetupMoteBLE(void){
		P9SEL |= (BIT5 | BIT4);	
		UCA2CTL1 |= UCSWRST;        // **Put state machine in reset**
		UCA2CTL1 |= UCSSEL_2;       // SMCLK
		UCA2BR0 = 0xD9;             // 25MHz 115200 (see User's Guide)
		UCA2BR1 = 0x00;             // 25MHz 115200
		UCA2MCTL |= UCBRS_1 + UCBRF_0; // Modulation UCBRSx=1, UCBRFx=0
		UCA2CTL1 &= ~UCSWRST;       // **Initialize USCI state machine**
	}

#endif /*UART_A2*/

/***********************************************************************/

#if UART_A0 == 1



#endif /*UART_A0*/
#if UART_A1 == 1

	void vSendMessageUART(unsigned char *pcBuffer, unsigned int uiLen){
		while(UCA1IE & UCTXIE)	//If interrupt still enabled
				__delay_cycles(100); //Sending message in progress w8?
		pcUARTTXBuffer = pcBuffer;		//Set Up UART buffer
		uiTXNumBytes = (--uiLen);			//Set up and decrement bit count
		UCA1TXBUF = *pcUARTTXBuffer;	//Send Decremented byte and place it ready to transmit
		pcUARTTXBuffer++;				//increment buffer pointer
		UCA1IE |= UCTXIE;				//Start Transmitting
	}

#endif /*UART_A1*/
#if UART_A2 == 1



#endif /*UART_A2*/

/***********************************************************************/

#if UART_A0 == 1



#endif /*UART_A0*/
#if UART_A1 == 1

	void vReceiveMessageUART(unsigned char *pcBuffer, int iMaxLen){
		while(UCA1IE & UCRXIE)	//If interrupt still enabled
			__delay_cycles(100); //Receiving message in progress w8?
		pcUARTRXBuffer = pcBuffer;
		uiRXNumBytes = iMaxLen;
		UCA1IE |= UCRXIE;
	}

#endif /*UART_A1*/
#if UART_A2 == 1



#endif /*UART_A2*/

/***********************************************************************/

#if UART_A0 == 1
	

#endif /*UART_A0*/
#if UART_A1 == 1

	void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void){
		if(UCA1IFG & UCRXIFG && UCA1IE & UCRXIE){	// Vector 2 - RXIFG
			*pcUARTRXBuffer	= UCA1RXBUF;
			pcUARTRXBuffer++;
			uiRXNumBytes--;
			if(uiRXNumBytes == 0){
				UCA1IE &= ~UCRXIE;
			} 
		}
		if(UCA1IFG & UCTXIFG && UCA1IE & UCTXIE){	// Vector 4 - TXIFG
			if(uiTXNumBytes == 0){
				UCA1IE &= ~UCTXIE;
			} else{
				UCA1TXBUF = *pcUARTTXBuffer;
				pcUARTTXBuffer++;
				uiTXNumBytes--;
			}
		}
	}

#endif /*UART_A1*/
#if UART_A2 == 1

	

#endif /*UART_A2*/