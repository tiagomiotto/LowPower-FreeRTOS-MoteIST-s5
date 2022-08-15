
#define fibonnaciCycles1MS 1392
//Fibanacci calculation = 1922 gives between 24994 and 25007 timer cycles at SMCLK
// 1922 is giving 38% higher times than it is supposed to, so trying 1292
#ifdef __cplusplus
extern "C"
#endif
void setupTaskParameters();


// void vDummyTask(void *pvParameters);



struct taskProperties
{
    int taskNumber;
    int taskPriority;
    int taskWorstCaseExecuteTime;
    int xDelay;
    unsigned int xFibonnaciCycles;
    unsigned int xFibonnaciCyclesWorstCase;
    int *xPowerConsumptionTestIsWorstCase;
};
#ifdef __cplusplus
extern "C" 
#endif
struct taskProperties task1Properties;
#ifdef __cplusplus
extern "C"
#endif
struct taskProperties task2Properties;
#ifdef __cplusplus
extern "C"
#endif
struct taskProperties task3Properties;
