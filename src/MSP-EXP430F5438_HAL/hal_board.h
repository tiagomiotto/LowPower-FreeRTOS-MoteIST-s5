
#ifndef HAL_BOARD_H
#define HAL_BOARD_H

// LEDS
#define LED_PORT_DIR	P4DIR
#define LED_PORT_SEL	P4SEL
#define LED_PORT_OUT	P4OUT
#define LED_R         	BIT7
#define LED_O         	BIT6
#define LED_G		 	BIT5

#define LED_1		  LED_R
#define LED_2		  LED_O
#define LED_3		  LED_G


// Hirose b9-> I2C_data (U1-22)(HSDA); b10->I2C_clock(U1-21) (HSCL?) HSCLK(18) 
// HSDA -> P3.7
// HSCL -> P5.4
// HSCLK -> P9.3 (not mapped no MDA100)
///////////////////

#define I2C_DATA_PORT_DIR P3DIR
#define I2C_DATA_PORT_OUT P3OUT

#define I2C_CL_PORT_DIR P5DIR

#define CLK_PORT_DIR      P11DIR //outputs clocks to testpoints
#define CLK_PORT_OUT      P11OUT
#define CLK_PORT_SEL      P11SEL

/*----------------------------------------------------------------
 *                  Function Prototypes
 *----------------------------------------------------------------
 */
extern void halBoardOutputSystemClock(void);
extern void halBoardStopOutputSystemClock(void);
extern void halBoardInit(void);
void hal430SetSystemClock(unsigned long req_clock_rate, unsigned long ref_clock_rate);



void hal_i2c_init_slave(unsigned char my_addr);
void hal_setup_leds(void);
void hal_toggle_leds(void);
void hal_toggle_led(unsigned char led);
void hal_setup_mote_cc2420(void); //USCIB0


//Interrupt Service Routines
//void __attribute__( (interrupt(USCI_A0_VECTOR)) ) USCI_A0_ISR( void )

#endif /* HAL_BOARD_H */
