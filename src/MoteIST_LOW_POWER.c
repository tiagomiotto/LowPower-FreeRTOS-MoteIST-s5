/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "MSP430Timer.h"
#include "MoteIST_LOW_POWER.h"
#include "hal_MSP-430F5438-MoteIST.h"
#include "./MoteModules/include/MOTEIST_serial.h"
#include "FreeRTOSConfig.h"
#include "config.h"

/* Holds the maximum number of ticks that can be suppressed - which is
basically how far into the future an interrupt can be generated. Set during
initialisation. */
TickType_t xMaximumPossibleSuppressedTicks = maxSUPPRESSED_TICKS;

uint16_t reloadTime = 0;

int contadorIdle = 0;


//Global variables stablished during the setup
static int numberOfTasks  = 0;
static int* taskWorstCaseComputeTime;
static int  availableFrequencyLevels;
static int* frequencyStages;
static int* taskDeadlines;
static int mode;
static int currentFrequencyLevel;

static int* d_i;
static int* c_lefti;

static int auxiliarValue =0;
// Default values for frequency levels
const int default_frequencyStages[] = {24, 21, 18, 15, 12, 9, 6, 3};
const int default_availableFrequencyLevels = 8;

static int frequencyChosenSVS = 0;





// 	LPC_GPIO1->FIOPIN = (0 << 23);
// #define configUSE_TICKLESS_IDLE 2

#if ( configUSE_TICKLESS_IDLE == 2 )
void vApplicationSleep(TickType_t xExpectedIdleTime)
{
	uint16_t ulCounterValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
	TickType_t xModifiableIdleTime;

	uint16_t ulReloadValue;
	// LED_PORT_OUT ^= LED_R;

	/* Make sure the SysTick reload value does not overflow the counter. */
	if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
	{
		xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
	}

	/* Stop the Tick Timer momentarily.  The time the Tick Timer is stopped for
	is accounted for as best it can be, but using the tickless mode will
	inevitably result in some tiny drift of the time maintained by the
	kernel with respect to calendar time. */
	vPortDisableTimer0();

	/* Calculate the reload value required to wait xExpectedIdleTime
	tick periods.  -1 is used because this code will execute part way
	through one of the tick periods. */


	// if (ulReloadValue > ulStoppedTimerCompensation)
	// {
	// 	ulReloadValue -= ulStoppedTimerCompensation;
	// }


	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	__disable_interrupt();

	/* The tick flag is set to false before sleeping.  If it is true when sleep
	mode is exited then sleep mode was probably exited because the tick was
	suppressed for the entire xExpectedIdleTime period. */
	ulTickFlag = pdFALSE;

	/* Adjust the TIM2 value to take into account that the current time
	slice is already partially complete. */
	ulReloadValue = ulReloadValueForOneTick * xExpectedIdleTime - vPortGetCounterTimer0();

	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if (eTaskConfirmSleepModeStatus() == eAbortSleep)
	{
		/* Restart from whatever is left in the count register to complete
		this tick period. */
		vPortEnableTimer0();
		reloadTime = -10;
		/* Re-enable interrupts - see comments above the cpsid instruction()
		above. */
		__enable_interrupt();

	}
	else
	{

		/* Adjust the Timer1 value to take into account that the current time
		slice is already partially complete by Timer2. */
		ulReloadValue = (ulReloadValueForOneTickTicklessTimer * xExpectedIdleTime) - (vPortGetCounterTimer0() / timer1ACLK_DIVIDER );
		
		if (ulReloadValue == 6450) {
			hal_toggle_led(LED_3);
			while (1);
		}	


		/* If the . */
		// if (ulReloadValue <= 0) {
		// 	hal_toggle_led(LED_3);
		// 	while (1);
		// }

		reloadTime = ulReloadValue;
		
		//TIME 1
		vPortSetReloadTimer1(ulReloadValue);


		/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
		set its parameter to 0 to indicate that its implementation contains
		its own wait for interrupt or wait for event instruction, and so wfi
		should not be executed again.  However, the original expected idle
		time variable must remain unmodified, so a copy is taken. */

		xModifiableIdleTime = xExpectedIdleTime;
		 // __bis_SR_register( LPM3_bits + GIE );
		configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
		// if (xModifiableIdleTime > 0)
		// {
		// 	// __nop(); //wait FOR INTERRUPT?
		// }
		configPOST_SLEEP_PROCESSING(xExpectedIdleTime);



		//  Re-enable interrupts to allow the interrupt that brought the MCU
		// out of sleep mode to execute immediately.  see comments above
		// __disable_interrupt() call above. 
		// __enable_interrupt();

		/* Disable interrupts again because the clock is about to be stopped
		and interrupts that execute while the clock is stopped will increase
		any slippage between the time maintained by the RTOS and calendar
		time. */
		__disable_interrupt();

		/* Disable the SysTick clock without reading the
		portNVIC_SYSTICK_CTRL_REG register to ensure the
		portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
		the time the SysTick is stopped for is accounted for as best it can
		be, but using the tickless mode will inevitably result in some tiny
		drift of the time maintained by the kernel with respect to calendar
		time*/
		vPortDisableTimer1();

		/* Determine if the SysTick clock has already counted to zero and
		been set back to the current reload value (the reload back being
		correct for the entire expected idle time) or if the SysTick is yet
		to count to zero (in which case an interrupt other than the SysTick
		must have brought the system out of sleep mode). */
		if (ulTickFlag != pdFALSE)
		{
			/* The tick interrupt is already pending, and the SysTick count
			reloaded with ulReloadValue.  Reset the
			portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
			period. */
			ulCounterValue = ulReloadValueForOneTickTicklessTimer - vPortGetCounterTimer1();

			/* Trap under/overflows before the calculated value is used. */
			configASSERT(ulCounterValue >= 0);


			/* As the pending tick will be processed as soon as this
			function exits, the tick value maintained by the tick is stepped
			forward by one less than the time spent waiting. */
			ulCompleteTickPeriods = xExpectedIdleTime - 1UL;



		}
		else /// CORRECT HERE - IT SHOULD HAVE A WAY OF ADJUSTING THE COMPLETED PERIODS TO TIMER 1 - STILL NOT DOING IT
		{

			/* Something other than the tick interrupt ended the sleep.
			Work out how long the sleep lasted rounded to complete tick
			periods (not the ulReload value which accounted for part
			ticks). */
			ulCompleteTickPeriods = (vPortGetCounterTimer1()) / ulReloadValueForOneTickTicklessTimer;

			/* The reload value is set to whatever fraction of a single tick
			period remains. */
			ulCounterValue = (vPortGetCounterTimer1()) - (ulCompleteTickPeriods * ulReloadValueForOneTickTicklessTimer);

			if (ulCounterValue == 0)
			{
				/* There is no fraction remaining. */
				ulCounterValue = ulReloadValueForOneTick;
				ulCompleteTickPeriods++;
			}

		}



		vTaskStepTick(ulCompleteTickPeriods);
		// if(ulCompleteTickPeriods==79) 

		/* Exit with interrupts enabled. */
		__enable_interrupt();

		/* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
		again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
		value. */
		vPortEnableTimer0();
		// hal_toggle_led(LED_2);


	}

	return;
}


#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void )
    {
        
        // if(TA1IV)
        ulTickFlag = pdTRUE;
        // hal_toggle_led(LED_3);
        __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF + GIE );
        //     __nop();
    }
#endif


/* Static voltage Scaling Implementation
**
** Mode 0: RM
	Mode 1 EDF*/





static int frequencyLevelSelector(int level) {

	if (level > availableFrequencyLevels - 1) level = availableFrequencyLevels - 1;
	else if (level < 0) level = 0;
	if(level == currentFrequencyLevel) return frequencyStages[level];
	
	currentFrequencyLevel = level;
	hal430SetSystemClock( frequencyStages[level] * 1000000, 32768 );
	return frequencyStages[level];
}


void setupDVFS(int main_numberOfTasks, int* main_taskWorstCaseComputeTime, int* main_taskDeadlines, int main_availableFrequencyLevels, int* main_frequencyStages, int main_mode) {

	numberOfTasks = main_numberOfTasks;
	taskWorstCaseComputeTime = (int *)pvPortMalloc(numberOfTasks * sizeof(int));
	memcpy(taskWorstCaseComputeTime, main_taskWorstCaseComputeTime, sizeof(int) * numberOfTasks);

	taskDeadlines = (int *)pvPortMalloc(numberOfTasks * sizeof(int));
	memcpy(taskDeadlines, main_taskDeadlines, sizeof(int) * numberOfTasks);
	mode = main_mode;
	availableFrequencyLevels = main_availableFrequencyLevels;
	frequencyStages = (int *)pvPortMalloc(availableFrequencyLevels * sizeof(int));
	memcpy(frequencyStages, main_frequencyStages, sizeof(int) * availableFrequencyLevels);
}


// void default_setupDVFS(int main_numberOfTasks, int* main_taskWorstCaseComputeTime, int* main_taskDeadlines, int main_mode){
// 	numberOfTasks = main_numberOfTasks;
// 	taskWorstCaseComputeTime = (int *)pvPortMalloc(numberOfTasks * sizeof(int));
// 	memcpy(taskWorstCaseComputeTime, main_taskWorstCaseComputeTime, sizeof(int) * numberOfTasks);

// 	taskDeadlines = (int *)pvPortMalloc(numberOfTasks * sizeof(int));
// 	memcpy(taskDeadlines, main_taskDeadlines, sizeof(int) * numberOfTasks);

// 	mode = main_mode;
// 	availableFrequencyLevels = default_availableFrequencyLevels;
// 	frequencyStages = default_frequencyStages;
// }



// Changed the amount of frequency levels and the frequency levels array into local variables
// Need to implement the select frequency here as well

#ifdef __cplusplus
extern "C"
#endif
int

staticVoltageScalingFrequencyLevelSelector()
{

	float alphaToTest = 0.0f;
	bool validAlpha = true;
	int i = 0;
	int selectedFrequencyLevel = 0;

	//Catch error if setup not implemented
	if (numberOfTasks == 0) {
		return -2;
	}

	/* Even if it fails in the max frequency, will return 0 and operate at max frequency
		If it is valid through all levels the second condition will act and stop the loop*/
	// if (mode == 0) //RM
	// {
	while (validAlpha && i < availableFrequencyLevels)
	{
		alphaToTest = (frequencyStages[i] * 1.0f) / (frequencyStages[0] * 1.0f);
		validAlpha = staticVoltageScalingRM_Test(alphaToTest);
		if (validAlpha)
			i++;
	}
	if (!validAlpha && i == 0) return -1;
	selectedFrequencyLevel = i - 1;
	// currentFrequencyLevel = selectedFrequencyLevel;
	frequencyLevelSelector(selectedFrequencyLevel);
	return selectedFrequencyLevel;
	// }

	// if (mode == 1) //EDF
	// {
	// 	while (validAlpha && i < availableFrequencyLevels)
	// 	{
	// 		alphaToTest = (frequencyStages[i] * 1.0f) / (frequencyStages[0] * 1.0f);
	// 		validAlpha = staticVoltageScalingEDF_Test(alphaToTest);
	// 		if (validAlpha)
	// 			i++;
	// 	}
	// 	if(!validAlpha && i==0) return -1;
	// 	selectedFrequencyLevel = i - 1;
	// 	currentFrequencyLevel = selectedFrequencyLevel;
	// 	return frequencyLevelSelector(selectedFrequencyLevel);
	// }
	// return 0;
}

bool staticVoltageScalingRM_Test(float alpha)
{

	int currentTaskInTest = 0;
	float computeTimeSum = 0;
	float ceilAux = 0;
	int i = 0;

	while (1)
	{
		for ( i = 0; i < currentTaskInTest + 1; i++)
		{
			ceilAux = (taskDeadlines[currentTaskInTest] / 1.0f) / (taskDeadlines[i] / 1.0f);
			if (ceilAux != (int) ceilAux) ceilAux = (int) ceilAux + 1; //If true, there is floating point, hence we remove and add one to ceil the value
			computeTimeSum += ceilAux * taskWorstCaseComputeTime[i]; //ceil
		}

		if (alpha * taskDeadlines[currentTaskInTest] >= computeTimeSum)
		{

			if (currentTaskInTest + 1 == numberOfTasks)
				return true; //Alpha is valid and deadlines are not violated

			/* Reset the sum and test the next Task*/
			computeTimeSum = 0;
			currentTaskInTest++;
		}
		else
			return false;
	}
}


/* Deadlines array has to be an array of pointers, to allow for the tasks to change the values inside the array
** during execution by changing the value of the pointer directly, intead of accessing the array
**
** To note: This function is not optimized for a huge amount of tasks, but having alarge amount of tasks
** would break all the algorithms, therefore optimization here is not that important
*/
static int findNextDeadline(int *deadlinesArray, int currentTick)
{
	int nextDeadline = INT16_MAX; // The element in the array is a pointer to the deadline of the First task
	int i = 0;
	for (i = 0; i < numberOfTasks; i++)
	{
		if (nextDeadline > deadlinesArray[i] && deadlinesArray[i] > currentTick)
			nextDeadline = deadlinesArray[i];
	}
	return nextDeadline;
}

/* Cycle Conserving DVS Implementation
   Only Implemented for RM
*/
// Changed the numberOfTasks to a local variable
#if LOW_POWER_MODE == 2
int setupCycleConservingDVS()
{


	frequencyChosenSVS = staticVoltageScalingFrequencyLevelSelector();
	if (frequencyChosenSVS == -1)
		return -1;
	int i = 0;

	d_i = (int *)pvPortMalloc(numberOfTasks * sizeof(int));
	c_lefti = (int *)pvPortMalloc(numberOfTasks * sizeof(int));

	if (frequencyChosenSVS < 0)
		return -1;
	for (i = 0; i < numberOfTasks; i++)
	{
		c_lefti[i] = taskWorstCaseComputeTime[i];
		d_i[i] = 0;
	}

	return frequencyChosenSVS;
}

// Changed number of Tasks to local
//  Changed deadlines to local
static int cycleConservingDVSFrequencySelector(int currentTick)
{
	// No need to convert into cycles, as the worst case computation time is in Ticks
	int maxTicksUntilNextDeadline = findNextDeadline(taskDeadlines, currentTick) - currentTick;

	int totalD = 0;

	int desiredFrequencyLevel = 0;

	int i = 0;

	for (i = 0; i < numberOfTasks; i++)
	{
		totalD += d_i[i];
	}

	float minimumFrequency = ((totalD * 1.0f) / maxTicksUntilNextDeadline * 1.0f) * frequencyStages[0];

	/* Can be optimized if we have a lot of frequencies to choose from, by only searching the bottom or top frequencies
	** Here it is not necessary as we have only 10
	*/
	if (minimumFrequency > frequencyStages[1])
	{
		desiredFrequencyLevel = 0; // If bigger than the 2 available frequency, we can only use the base
	}
	else
	{
		for (i = 1; i < availableFrequencyLevels; i++)
		{
			if (minimumFrequency > frequencyStages[i])
				break;
			desiredFrequencyLevel++;
		}
	}
	// if(desiredFrequencyLevel == 3) LED_PORT_OUT ^= LED_3;
	// if(desiredFrequencyLevel & 4 ) LED_PORT_OUT ^= LED_1;
	// if(desiredFrequencyLevel & 2 ) LED_PORT_OUT ^= LED_2;
	// if(desiredFrequencyLevel & 1 ) LED_PORT_OUT ^= LED_3;


	// Esta a causar erros aqui for some reason (devia ser assim o codigo antigo quando calculava
	// para ir pra o nivel maximo nao ia, fazendo com que deadlines fossem missed)
	// currentFrequencyLevel = desiredFrequencyLevel;
	frequencyLevelSelector(desiredFrequencyLevel);
	return maxTicksUntilNextDeadline;
}

static void cycleConservingDVSAllocateCycles(int k)
{
	int i = 0;
	for (i = 0; i < numberOfTasks; i++)
	{
		if (c_lefti[i] < k)
		{
			d_i[i] = c_lefti[i];
			k = k - c_lefti[i];
		}
		else
		{
			d_i[i] = k;
			k = 0;
		}
	}
}

void checkForOtherTasksReady(int* deadlinesArray, int currentTick){
	
	int i = 0;
	for (i = 0; i < numberOfTasks; i++)
	{
		if (deadlinesArray[i] <= currentTick)
			c_lefti[i] = taskWorstCaseComputeTime[i];
	}
	
}

int cycleConservingDVSTaskReady(int taskNumber, int currentTick, int taskNextExecution)
{
	taskDeadlines[taskNumber] = taskNextExecution;
	c_lefti[taskNumber] = taskWorstCaseComputeTime[taskNumber];
	

	//Any low priority task in ready state?????????? Maybe we should check
	//To populate c_lefti
	checkForOtherTasksReady(taskDeadlines, currentTick);

	int s_m = findNextDeadline(taskDeadlines, currentTick) - currentTick;

	// float ceilAux = (s_m * (frequencyStages[frequencyChosenSVS] * 1.0) / (frequencyStages[0] * 1.0));
	// if (ceilAux != (int)ceilAux)
	// 	ceilAux = (int)ceilAux + 1; // If true, there is floating point, hence we remove and add one to ceil the value
	int aux = 0;
	int s_j =  (s_m * (frequencyStages[frequencyChosenSVS] * 1.0f) / (frequencyStages[0] * 1.0f));;


	

	// int s_j = ceil(s_m * (frequencyStages[currentFrequencyLevel] * 1.0) / (frequencyStages[0] * 1.0));
	cycleConservingDVSAllocateCycles(s_j);
	auxiliarValue = s_j;
	aux = cycleConservingDVSFrequencySelector(currentTick);
	return aux;
}

int cycleConservingDVSTaskComplete(int taskNumber, int currentTick)
{
	c_lefti[taskNumber] = 0;
	d_i[taskNumber] = 0;

	return cycleConservingDVSFrequencySelector(currentTick);
}
#endif
int getCurrentFrequency() {

	return auxiliarValue;
	// return frequencyStages[currentFrequencyLevel];
}


int pow(int base, int power) {
	int result = base, i;
	for (i = 1; i < power; ++i)
	{
		result *= result;
	}
	return result;
}

bool sufficientSchedulabilityTest()
{
	float utilisation = 0, utilisationBound = 0;
	int i = 0;

	utilisationBound = numberOfTasks * (pow(2, (1.0 / numberOfTasks)) - 1);

	for (i = 0; i < numberOfTasks; i++)
	{
		utilisation += taskWorstCaseComputeTime[i] / taskDeadlines[i];
	}

	if (utilisation > 1.0 || utilisation > utilisationBound)
		return false;
	return true;
}

void vTaskStartLowPowerScheduller(int main_numberOfTasks, int *main_taskWorstCaseComputeTime, int *main_taskDeadlines, int main_availableFrequencyLevels, int *main_frequencyStages, int main_mode)
{
	int selectedLevel = 0;
	if (!sufficientSchedulabilityTest(main_numberOfTasks, main_taskWorstCaseComputeTime, main_taskDeadlines))
	{

		LED_PORT_OUT ^= LED_2;
		LED_PORT_OUT ^= LED_3;

		return;
	}

	switch (main_mode)
	{
	// Sleep on idle
	case 0:
		dvfsMode = 0;
		break;
	// SVS
	case 1:
		setupDVFS(main_numberOfTasks, main_taskWorstCaseComputeTime, main_taskDeadlines, main_availableFrequencyLevels, main_frequencyStages, main_mode);
		dvfsMode = 1;
		selectedLevel = staticVoltageScalingFrequencyLevelSelector();
		//if(selectedLevel==0) LPC_GPIO1->FIOPIN = (1 << 23);

		break;
	// Cycle Conserving no Tickless
	case 2:
		setupDVFS(main_numberOfTasks, main_taskWorstCaseComputeTime, main_taskDeadlines, main_availableFrequencyLevels, main_frequencyStages, main_mode);
		dvfsMode = 2;
		#if LOW_POWER_MODE == 2
		setupCycleConservingDVS();
		#endif
		break;

	}
}





// /* Cycle Conserving DVS EDF
// */

// #if numberOFTASKS != 3
// #error Correct here to increase the number of Tasks, as this was a POC aimed at a system with 3 tasks
// #endif

// float U_1 = 0, U_2 = 0, U_3 = 0;
// float *U_i[3] = {&U_1, &U_2, &U_3};

// int cycleConservingDVSFrequencySelectorEDF()
// {

// 	float totalU = 0;
// 	int i = 0;

// 	for ( i = 0; i < numberOFTASKS; i++)
// 	{
// 		totalU += *U_i[i];
// 	}

// 	float minimumFrequency = totalU * frequencyLevels[0];

// 	/* Can be optimized if we have a lot of frequencies to choose from, by only searching the bottom or top frequencies
// 	** Here it is not necessary as we have only 10
// 	*/
// 	if (minimumFrequency > frequencyLevels[1])
// 		return 0; //If bigger than the 2 available frequency, we can only use the base

// 	int desiredFrequencyLevel = 0;
// 	for ( i = 1; i < availableFrequencyLevels; i++)
// 	{
// 		if (minimumFrequency > frequencyLevels[i])
// 			break;
// 		desiredFrequencyLevel++;
// 	}
// 	frequencyLevelSelector(desiredFrequencyLevel);
// 	return desiredFrequencyLevel;
// }

// void cycleConservingDVSTaskReadyEDF(int taskNumber, int currentTick)
// {
// 	int auxDeadline = *deadlines[taskNumber];
// 	*U_i[taskNumber] = mainWorstCaseComputeTime[taskNumber] * auxDeadline;
// 	cycleConservingDVSFrequencySelectorEDF(currentTick);
// }

// /* This one requires us to keep track of the execution time of the task.
// ** Maybe should implement this for all the algorithms?
// ** Or something available to all the the task
// */
// void cycleConservingDVSTaskCompleteEDF(int taskNumber, int actualTaskExecutionTicks)
// {
// 	int auxDeadline = *deadlines[taskNumber];
// 	*U_i[taskNumber] = actualTaskExecutionTicks / auxDeadline;
// 	cycleConservingDVSFrequencySelectorEDF();
// }
