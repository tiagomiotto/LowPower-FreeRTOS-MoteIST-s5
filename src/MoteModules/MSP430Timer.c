/* Hardware includes. */
#include "MSP430Timer.h"
#include "FreeRTOS.h"

void vPortSetupTimer0(void){
	    /* Ensure the timer is stopped. */
    TA0CTL = 0;
	//__delay_cycles(50);
    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Enable the interrupts. */
    TA0CCTL0 |= CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;

}

void vPortSetupReloadTimer0(uint16_t reload){
    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = reload;
}

void vPortDisableTimer0(void){
	/* Clear the MC_X bit */
    TA0CTL &= ~(0b11 << 4);

    //      Enable the interrupts. 
    // TA0CCTL0 &= ~(CCIE);


}
void vPortEnableTimer0(void){

    /* Up mode. */
    TA0CTL |= MC_1;
}
void vPortResetTimer0(void){
	 //    /* Start up clean. */
    TA0CTL |= TACLR;
}
uint16_t vPortGetCounterTimer0(void){
	return TA0R;
}
void vPortSetCounterTimer0(uint16_t count){
	TA0R = count;
}
void vPortSetReloadTimer0(uint16_t reload){
	/* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = reload;
}

uint16_t vPortGetReloadTimer0(){
	/* Set the compare match value according to the tick rate we want. */
    return TA0CCR0;
}


/* ##### TIMER 1 */

void vPortSetupTimer1(int reloadTime){
	    /* Ensure the timer is stopped. */
    TA1CTL |= MC_0 + TACLR;
	//__delay_cycles(50);
    /* Run the timer from the ACLK. */
    TA1CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA1CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA1CCR0 = (int) reloadTime;

    /* Enable the interrupts. */
    TA1CCTL0 |= CCIE;

    /* Start up clean. */
    TA1CTL |= TACLR;

    /* Divide the clock frequency by 8 */
    TA1CTL |= ID_3;

    /* Divide the Clock Frequency by another 8 */
    TA1EX0 = TAIDEX_7;

    /* Up mode. */
    TA1CTL |= MC_1;

}

void vPortSetupReloadTimer1(uint16_t reload){
    /* Set the compare match value according to the tick rate we want. */
    TA1CCR0 = reload;
}

void vPortDisableTimer1(void){
	/* Clear the MC_X bit */
    TA1CTL |= MC_0;
    //      Enable the interrupts. 
    // TA0CCTL0 &= ~(CCIE);


}
void vPortEnableTimer1(void){
	// /* Ensure the timer is stopped. */
 //    TA0CTL = 0;

 //    /* Run the timer from the ACLK. */
 //    TA0CTL = TASSEL_1;

 //    /* Enable the interrupts. */
 //    TA0CCTL0 |= CCIE;

 //    /* Start up clean. */
 //    TA0CTL |= TACLR;

    // /* Up mode. */
    // TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Up mode. */
    TA1CTL |= MC_1;
}
void vPortResetTimer1(void){
	 //    /* Start up clean. */
    TA1CTL |= TACLR;
}
uint16_t vPortGetCounterTimer1(void){
	return TA1R;
}
void vPortSetCounterTimer1(uint16_t count){
	TA1R = count;
}
void vPortSetReloadTimer1(uint16_t reload){
	/* Set the compare match value according to the tick rate we want. */
    TA1CCR0 = reload;
}

uint16_t vPortGetReloadTimer1(){
	/* Set the compare match value according to the tick rate we want. */
    return TA1CCR0;
}
