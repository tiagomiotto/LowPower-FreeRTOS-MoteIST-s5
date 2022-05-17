/*
 * uart.h 
 *
 *  Created on: May 17, 2015
 *      Author: Diogo Guerra
 */

//READ ME		READ ME			READ ME			READ ME			READ ME
//Interface Rate is configured in uart.c

#ifndef UART_H_
#define UART_H_

#define UART_A0 1
#define UART_A2 1


#if UART_A0 == 1

void vHalSetupMoteUSB(void);

#endif
#if UART_A1 == 1

void vInstallPeripheralUART(void);
void vSendMessageUART(unsigned char *pcBuffer, unsigned int uiLen);
void vReceiveMessageUART(unsigned char *pcBuffer, int iMaxLen);

#endif
#if UART_A2 == 1

void vHalSetupMoteBLE(void);

#endif

#endif /* UART_H_ */
