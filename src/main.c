/*
 * FreeRTOS Kernel V10.4.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The documentation page for this demo available on http://www.FreeRTOS.org
 * documents the hardware configuration required to run this demo.  It also
 * provides more information on the expected demo application behaviour.
 *
 * main() creates all the demo application tasks, then starts the scheduler.
 * A lot of the created tasks are from the pool of "standard demo" tasks.  The
 * web documentation provides more details of the standard demo tasks, which
 * provide no particular functionality but do provide good examples of how to
 * use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks, interrupts tests
 * and timers are defined and/or created within this file:
 *
 * "LCD" task - The LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the LCD and therefore ensures access to the LCD is
 * always serialised and there are no mutual exclusion issues.  When a task or
 * an interrupt wants to write to the LCD, it does not access the LCD directly
 * but instead sends the message to the LCD task.  The LCD task then performs
 * the actual LCD output.  This mechanism also allows interrupts to, in effect,
 * write to the LCD by sending messages to the LCD task.
 *
 * The LCD task is also a demonstration of a 'controller' task design pattern.
 * Some tasks do not actually send a string to the LCD task directly, but
 * instead send a command that is interpreted by the LCD task.  In a normal
 * application these commands can be control values or set points, in this
 * simple example the commands just result in messages being displayed on the
 * LCD.
 *
 * "Button Poll" task - This task polls the state of the 'up' key on the
 * joystick input device.  It uses the vTaskDelay() API function to control
 * the poll rate to ensure debouncing is not necessary and that the task does
 * not use all the available CPU processing time.
 *
 * Button Interrupt - The select button on the joystick input device is 
 * configured to generate an external interrupt.  The handler for this interrupt 
 * sends a message to LCD task, which then prints out a string to say the 
 * joystick select button was pressed.
 *
 * Idle Hook - The idle hook is a function that is called on each iteration of
 * the idle task.  In this case it is used to place the processor into a low
 * power mode.  Note however that this application is implemented using standard
 * components, and is therefore not optimised for low power operation.  Lower
 * power consumption would be achieved by converting polling tasks into event
 * driven tasks, and slowing the tick interrupt frequency, etc.
 *
 * "Check" callback function - Called each time the 'check' timer expires.  The
 * check timer executes every five seconds.  Its main function is to check that 
 * all the standard demo tasks are still operational.  Each time it executes it 
 * sends a status code to the LCD task.  The LCD task interprets the code and 
 * displays an appropriate message - which will be PASS if no tasks have 
 * reported any errors, or a message stating which task has reported an error.
 *
 * "Reg test" tasks - These fill the registers with known values, then check
 * that each register still contains its expected value.  Each task uses
 * different values.  The tasks run with very low priority so get preempted
 * very frequently.  A check variable is incremented on each iteration of the
 * test loop.  A register containing an unexpected value is indicative of an
 * error in the context switching mechanism and will result in a branch to a
 * null loop - which in turn will prevent the check variable from incrementing
 * any further and allow the check timer callback (described a above) to 
 * determine that an error has occurred.  The nature of the reg test tasks 
 * necessitates that they are written in assembly code.
 *
 * Tick hook function - called inside the RTOS tick function, this simple 
 * example does nothing but toggle an LED.
 *
 * *NOTE 1* vApplicationSetupTimerInterrupt() is called by the kernel to let
 * the application set up a timer to generate the tick interrupt.  In this
 * example a timer A0 is used for this purpose.
 *
*/

/* Standard includes. */
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "testApplication.h"

/* Hardware includes. */
#include "msp430.h"
#include "hal_MSP-430F5438-MoteIST.h"
#include "./MoteModules/include/MOTEIST_serial.h"
#include "MoteIST_LOW_POWER.h"


/*
 * Configures clocks, LCD, port pints, etc. necessary to execute this demo.
 */
static void prvSetupHardware( void );
void vTaskLedToggle(int taskNumber);

 void vDummyTask(void *pvParameters);
 void vDummyTask3(void *pvParameters);
 void vDummyTask2(void *pvParameters);
 long fibonnacciCalculation(long cycles);

 bool consumptionTest = false;
 #define lowPowerMode 1
/*-----------------------------------------------------------*/

// static void vBlink(void * pvParameters){
// 	int* param = (int*) pvParameters;
// 	char buffer[50];
// 	while(1){
// 		if(*param ==3) hal_toggle_led(LED_1);
// 		vTaskDelay( 250 );
// 		hal_toggle_led(LED_2);
// 		hal_toggle_led(LED_3);
// 		vTaskDelay( 250 );
// 		hal_toggle_led(LED_2);
// 		vTaskDelay( 250 );
// 		hal_toggle_led(LED_2);
// 		hal_toggle_led(LED_3);
// 		vTaskDelay( 250 );
// 		hal_toggle_led(LED_2);
// 		sprintf(buffer,"This is MoteIST! I am listening %d...\r\n", *param );
// 		xSendSerialMessage(buffer);
// 	}
// }
void initTimer_A(void)
{
    //Timer0_A3 Configuration

    TA1CCR0 = 0; //Initially, Stop the Timer
    /* Clear everything to start with. */
    TA1CTL |= TACLR;
    TA1CCTL0 |= CCIE; //Enable interrupt for CCR0.
    TA1CTL = TASSEL_2 + ID_0 + MC_2; //Select SMCLK, SMCLK/1, Up Mode
}
uint16_t vGetCounterTimer1(void){
    return TA1R;
}

/*-----------------------------------------------------------*/

void main( void )
{
	/* Configure the peripherals used by this demo application.  This includes
	configuring the joystick input select button to generate interrupts. */
	prvSetupHardware();
	hal_setup_leds();
	xSerialPortInit(200);

	
	setupTaskParameters();

		// int aux =37;
		// xTaskCreate( vExecutionTimeTesterTask, "Reg2", configMINIMAL_STACK_SIZE*4, &task1Properties, 4, NULL );

		xTaskCreate( vDummyTask, "Task1", configMINIMAL_STACK_SIZE*3, &task1Properties, task1Properties.taskPriority, NULL );
		xTaskCreate( vDummyTask, "Task2", configMINIMAL_STACK_SIZE*3, &task2Properties, task2Properties.taskPriority, NULL );
		xTaskCreate( vDummyTask, "Task3", configMINIMAL_STACK_SIZE*3, &task3Properties, task3Properties.taskPriority, NULL );

 		consumptionTest = true;
	if(!consumptionTest){
		    char buffer[100];
    sprintf(buffer, "[Starting]\r\n");
    xSendSerialMessage(buffer);
	} 


	

		/* Start the scheduler. */
		vTaskStartScheduler();

	/* If all is well then this line will never be reached.  If it is reached
	then it is likely that there was insufficient (FreeRTOS) heap memory space
	to create the idle task.  This may have been trapped by the malloc() failed
	hook function, if one is configured. */	
	for( ;; );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/



static void prvSetupHardware( void )
{
	taskDISABLE_INTERRUPTS();
	
	/* Disable the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;
  
	halBoardInit();

	LFXT_Start( XT1DRIVE_0 );
	hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

}
/*-----------------------------------------------------------*/


void vApplicationTickHook( void )
{
}

/*-----------------------------------------------------------*/

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
	//const unsigned short usACLK_Frequency_Hz = 32768;

	/* Ensure the timer is stopped. */
	TA0CTL = 0;

	/* Run the timer from the ACLK. */
	TA0CTL = TASSEL_1;

	/* Clear everything to start with. */
	TA0CTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

	/* Enable the interrupts. */
	TA0CCTL0 = CCIE;

	/* Start up clean. */
	TA0CTL |= TACLR;

	/* Up mode. */
	TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* Called on each iteration of the idle task.  In this case the idle task
	just enters a low(ish) power mode. */
	//__bis_SR_register( LPM1_bits + GIE );
	#if (config_SLEEP_ON_IDLE != 0)
    __bis_SR_register(LPM0_bits + GIE);
#endif
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues or
	semaphores. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pxTask;
	( void ) pcTaskName;
	
	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
long fibonnacciCalculation(long cycles)
{
    long a = 0, b = 1, i,j;

for (j = 0; j < 100; j++)
{
	    for (i = 0; i < cycles; i++)
    {
        long nextTerm = a + b;
        a = b;
        b = nextTerm;
    }
}

    return a;
}

/*-----------------------------------------------------------*/

// // Task for testing the pvParameters
// void vDummyTask3(void *pvParameters)
// {
//     char buffer[100];
//     /* Unpack parameters into local variables for ease of interpretation */
//     struct taskProperties *parameters = (struct taskProperties *) pvParameters;
//     const TickType_t xDelay = parameters->xDelay / portTICK_PERIOD_MS;
//     int baseCycles = parameters->xFibonnaciCycles;
//     int worstCaseCycles = parameters->xFibonnaciCyclesWorstCase;
//     int *isWorstCase = parameters->xPowerConsumptionTestIsWorstCase;
//     int taskNumber = parameters->taskNumber;

//     /* Define local variables for counting runs and delays */
//     TickType_t xLastWakeTime = 0;
//     int fibonnacciAuxiliar = 0;
//     int runNumber = 0;
//     int aux = 0;

//     sprintf(buffer, "[Task%d - Delay: %d]\r\n", taskNumber+1,xDelay);
//     xSendSerialMessage(buffer);
//     sprintf(buffer, "[Task%d - baseCycles: %d]\r\n", taskNumber+1,baseCycles);
//     xSendSerialMessage(buffer);
//     sprintf(buffer, "[Task%d - WorstCase: %d]\r\n", taskNumber+1,worstCaseCycles);
//     xSendSerialMessage(buffer);
//     sprintf(buffer, "[Task%d - taskNumber: %d]\r\n", taskNumber+1,isWorstCase[4]);
//     xSendSerialMessage(buffer);

//     uint16_t start=0,end=0;
//     initTimer_A();
//     for (;;)
//     {

//         start=vGetCounterTimer1();
//         fibonnacciAuxiliar = fibonnacciCalculation(1310);
//         end =vGetCounterTimer1();
//         sprintf(buffer, "[Task1 - timer for 1 tick: %u]\r\n", end-start);
//         xSendSerialMessage(buffer);
//         switch (taskNumber)
//         {
//         case 0:
//             hal_toggle_led(LED_1);
//             break;
//         case 1:
//             hal_toggle_led(LED_2);
//             break;
//         case 2:
//             hal_toggle_led(LED_3);
//             break;
//         }
        
//         vTaskDelayUntil(&xLastWakeTime, 100);
//     }

//     for (;;)
//     {

//                 switch (taskNumber)
//         {
//         case 0:
//             hal_toggle_led(LED_1);
//             break;
//         case 1:
//             hal_toggle_led(LED_2);
//             break;
//         case 2:
//             hal_toggle_led(LED_3);
//             break;
//         }

//         vTaskDelayUntil(&xLastWakeTime, xDelay);
//     }
// }
// Test application (Still not done)
void vDummyTask(void *pvParameters)
{

    char buffer[100];
    /* Unpack parameters into local variables for ease of interpretation */
    struct taskProperties *parameters = (struct taskProperties *)pvParameters;
    const TickType_t xDelay = parameters->xDelay / portTICK_PERIOD_MS;
    uint16_t baseCycles = parameters->xFibonnaciCycles;
    uint16_t worstCaseCycles = parameters->xFibonnaciCyclesWorstCase;
    int *isWorstCase = parameters->xPowerConsumptionTestIsWorstCase;
    int taskNumber = parameters->taskNumber;

    /* Define local variables for counting runs and delays */
    TickType_t xLastWakeTime = 0;
    volatile int fibonnacciAuxiliar = 0;
    int runNumber = 0;
    int aux = 0;

    if(!consumptionTest){
    	    sprintf(buffer, "[Task %d - Delay: %d]\r\n", taskNumber, xDelay);
    	xSendSerialMessage(buffer);
    }

    // sprintf(buffer, "[Task %d - baseCycles: %u]\r\n", taskNumber,baseCycles);
    // xSendSerialMessage(buffer);
    // sprintf(buffer, "[Task %d - WorstCase: %u]\r\n", taskNumber,worstCaseCycles);
    // xSendSerialMessage(buffer);
    // sprintf(buffer, "[Task %d - taskNumber: %d]\r\n", taskNumber,isWorstCase[4]);
    // xSendSerialMessage(buffer);
    uint16_t start=0,end=0;
    initTimer_A();
    // for (;;)
    // {

    //     start=vGetCounterTimer1();
    //     fibonnacciAuxiliar = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);
    //     end =vGetCounterTimer1();
    //     sprintf(buffer, "[Task1 - timer for %d tick: %u | end %u, start %u]\r\n", fibonnacciAuxiliar, end-start, end,start);
    //     xSendSerialMessage(buffer);
    //     switch (taskNumber)
    //     {
    //     case 0:
    //         hal_toggle_led(LED_1);
    //         break;
    //     case 1:
    //         hal_toggle_led(LED_2);
    //         break;
    //     case 2:
    //         hal_toggle_led(LED_3);
    //         break;
    //     }
    //             runNumber++;
    //     if (runNumber > 7)
    //         runNumber = 0;

    //     vTaskDelayUntil(&xLastWakeTime, 1000);
    // }
    for (;;)
    {

        // Cycle conserving task ready to run
        if (dvfsMode == 2)
            aux = cycleConservingDVSTaskReady(taskNumber, xTaskGetTickCount(), xLastWakeTime + xDelay);
        // If not testing light up leds
        if (!consumptionTest)
        {
			vTaskLedToggle(taskNumber);
			       			sprintf(buffer, "[Task%d - Start]\r\n", taskNumber);
        	xSendSerialMessage(buffer);
        }

        fibonnacciAuxiliar = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);
        


        // fibonnacciAuxiliar = fibonnacciCalculation(1000);
        if (!consumptionTest && fibonnacciAuxiliar!=0 )
        {
			vTaskLedToggle(taskNumber);
			sprintf(buffer, "[Task%d - timer for %d - time]\r\n", taskNumber, fibonnacciAuxiliar);
        	xSendSerialMessage(buffer);
        }
        runNumber++;
        if (runNumber > 7)
            runNumber = 0;

        // Cycle conserving task ready finished running
        if (dvfsMode == 2)
            cycleConservingDVSTaskComplete(taskNumber, xTaskGetTickCount());

        // If the deadline is missed suspend all and print out a code
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            hal_toggle_leds();
            vTaskSuspendAll();
        }
        // Explicar esse delay no modelo
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}
/*-----------------------------------------------------------*/

// void configureADC()
// {
//     // Configure ADC12
//     ADC12CTL0 = ADC12SHT0_2 | ADC12ON; // Cycle Sample Time, ADC On
//     ADC12CTL1 = ADC12SHP;              // Source clock is sample timer
//     ADC12CTL2 |= ADC12RES_2;           // 12-bit conversion
//                                        // ADC12IER0 |= ADC12IE0; // Interrupt MEM0
//     // ADC12MCTL0 |= ADC12INCH_4 | ADC12VRSEL_3; // Select A4, Vref = 2.5V
// }

// float sampleADC()
// {
//     ADC12CTL0 |= ADC12ENC | ADC12SC; // Sampling and conversion start
//     while (!(ADC12IFG & BIT0))
//         ;
//     return (ADC12MEM0 * (2500.0f / 4096.0f)) / 5.0f;
//     // __bis_SR_register(LPM0_bits + GIE); // LPM0, ADC12_ISR will force exit
//     // __no_operation(); // For debug only
// }

void vTaskLedToggle(int taskNumber){
	            switch (taskNumber)
            {
            case 0:
                hal_toggle_led(LED_1);
                break;
            case 1:
                hal_toggle_led(LED_2);
                break;
            case 2:
                hal_toggle_led(LED_3);
                break;
            }
}

