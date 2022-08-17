#include <stdbool.h>
// #include <math.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * The tick hook function.  This compensates
 * the tick count to compensate for frequency changes
 */
// const uint8_t mValues[] = {36, 33, 30, 27, 24, 18, 12, 9, 6};
// const int frequencyLevels[] = {96, 88, 80, 72, 64, 48, 32, 24, 16};
// const int staticTickIncrement[] = {0, 0, 0, 0, 0, 1, 2, 3, 5};
// const int periodicTickIncrement[] = {0, 11, 5, 3, 2, 0, 0, 0, 0};
// const int availableFrequencyLevels = 9;
// extern volatile int currentFrequencyLevel;
// extern volatile short periodicTickIncrementCount;
// extern volatile bool frequencyChanged;

void vApplicationSleep(TickType_t xExpectedIdleTime);
short dvfsMode;
#ifdef __cplusplus
extern "C" {
#endif 

void setupDVFS(int main_numberOfTasks, int* main_taskWorstCaseComputeTime, int* main_taskDeadlines, int main_availableFrequencyLevels, int* main_frequencyStages,int main_mode);
void default_setupDVFS(int main_numberOfTasks, int* main_taskWorstCaseComputeTime, int* main_taskDeadlines, int main_mode);

int
	staticVoltageScalingFrequencyLevelSelector(void);
bool staticVoltageScalingRM_Test(float alpha);

bool staticVoltageScalingEDF_Test(float alpha);

int setupCycleConservingDVS();
static int cycleConservingDVSFrequencySelector(int currentTick);
static void cycleConservingDVSAllocateCycles(int k);
int cycleConservingDVSTaskReady(int taskNumber, int currentTick, int taskNextExecution);
int cycleConservingDVSTaskComplete(int taskNumber, int currentTick);
void vTaskStartLowPowerScheduller(int main_numberOfTasks, int *main_taskWorstCaseComputeTime, int *main_taskDeadlines, int main_availableFrequencyLevels, int *main_frequencyStages, int main_mode);
bool sufficientSchedulabilityTest( );
int getCurrentFrequency();
// int cycleConservingDVSFrequencySelectorEDF();
// void cycleConservingDVSTaskReadyEDF(int taskNumber, int currentTick);
// void cycleConservingDVSTaskCompleteEDF(int taskNumber, int actualTaskExecutionTicks);

#ifdef __cplusplus
}
#endif