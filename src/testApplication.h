
#define fibonnaciCycles1MS 1922
//Fibanacci calculation = 1922 gives between 24994 and 25007 timer cycles at SMCLK

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
    long xFibonnaciCycles;
    long xFibonnaciCyclesWorstCase;
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
