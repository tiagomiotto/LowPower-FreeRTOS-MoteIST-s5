/*----------------------------------------------------------------------------------------------------------
* Module: 		Mote USB debugger
* Date: 		3/3/2015
* Developer:	Diogo Guerra
*
* Description:	MoteIST5++ USB module interface library for debugging purposes using a serial terminal
*---------------------------------------------------------------------------------------------------------*/

#ifndef SERIAL_H
#define SERIAL_H

//#define THREAD_SAFE 

 /* ---Hardware includes--- */
#include "msp430.h"
#include "hal_MSP-430F5438-MoteIST.h"
#include "hal_board.h"

/* ---Scheduler includes--- */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

/* ---Standard includes--- */
#include <string.h>

/* ---Other includes--- */
#include "uart.h"

//Functions Prototypes
portBASE_TYPE xSerialPortInit(unsigned portBASE_TYPE uxQueueLength );
portBASE_TYPE xSendSerialMessage(char * pcString);
portBASE_TYPE xSendSerialMessageN(char * pcString, int len);
portBASE_TYPE xReceiveSerialMessage(char * pcBuffer, unsigned int maxSize);
portBASE_TYPE xReceiveSerialMessageN(char * pcBuffer, unsigned int len);

#define PRINT( x ) xSendSerialMessageN((char *) x, 1)

#endif	/* SERIAL_H */	