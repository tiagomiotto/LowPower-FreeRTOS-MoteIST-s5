
#ifndef CONFIG_H
#define CONFIG_H


/* User defined defines */
#define CONSUMPTION_TEST 1

/* Sleep on the Idle Task Hook Controls */
#define configUSE_IDLE_HOOK				0
#define CPU_LOW_POWER_MODE 				0 // Number corresponds to LPM0, LPM1, LPM2, LPM3


/* Definitions for the Tick Timer */
#define portACLK_FREQUENCY_HZ			( ( TickType_t ) 32768 )
#define usACLK_Frequency_Hz				portACLK_FREQUENCY_HZ
#define ulReloadValueForOneTick	(portACLK_FREQUENCY_HZ / configTICK_RATE_HZ)


/* Tickless idle defines */
#define configUSE_TICKLESS_IDLE				0
#define timer1ACLK_DIVIDER					4
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP  			4
#define ulReloadValueForOneTickTicklessTimer	(ulReloadValueForOneTick / timer1ACLK_DIVIDER)
#define portSUPPRESS_TICKS_AND_SLEEP( xIdleTime ) vApplicationSleep( xIdleTime )
#define maxSUPPRESSED_TICKS ( 65535 / ulReloadValueForOneTickTicklessTimer)

#define configPRE_SLEEP_PROCESSING(x) __bis_SR_register( LPM0_bits + GIE )
#define configPOST_SLEEP_PROCESSING(x) hal_toggle_led(LED_2)

/* DVFS defines (1 for SVS, 2 for CCRTDVS) */
// #define LOW_POWER_MODE 2


#if (configUSE_IDLE_HOOK == 1) && (configUSE_TICKLESS_IDLE !=0)
	#error "The idle hook and the tickless are not compatible with each other, choose one"
#endif

#endif /* CONFIG_H */
