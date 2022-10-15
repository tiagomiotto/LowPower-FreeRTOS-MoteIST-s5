
/* Hardware includes. */
#include "msp430.h"
#include "stdint.h"


void vPortSetupTimer0(void);
void vPortSetupReloadTimer0(uint16_t reload);
void vPortDisableTimer0(void);
void vPortEnableTimer0(void);
void vPortResetTimer0(void);
uint16_t vPortGetCounterTimer0(void);
void vPortSetCounterTimer0(uint16_t count);
void vPortSetReloadTimer0(uint16_t reload);
uint16_t vPortGetReloadTimer0(void);



void vPortSetupTimer1(void);
void vPortSetReloadTimer1(uint16_t reload);
void vPortDisableTimer1(void);
void vPortEnableTimer1(void);
void vPortResetTimer1(void);
uint16_t vPortGetCounterTimer1(void);
void vPortSetCounterTimer1(uint16_t count);
uint16_t vPortGetReloadTimer1(void);


