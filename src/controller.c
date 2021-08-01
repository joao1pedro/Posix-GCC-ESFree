#include "controller.h"
#include "kb.h"

int statusLED = 1;
int systemHealth = 0;
float cpuMeansure = 0;
int flagPDA = 0;
int flagWeb = 0;

int maxValue1 = 5000;
int minValue1 = 1001;
int maxValue2 = 7050;
int minValue2 = 5001;


QueueHandle_t xQueue1;
QueueHandle_t xFieldBusQueue;
QueueHandle_t xEthernetQueue;
QueueHandle_t xRS232Queue;

TickType_t idleTask = 0;

struct
{
    int Data1, Data2;
} Data;

/* Plant Control Task */
void PlantControlTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    //int Data1, Data2 ,sendData1, sendData2;
    int sendData1, sendData2;

    /* Queue init */
    xQueue1 = xQueueCreate(10, sizeof(int));
    xFieldBusQueue = xQueueCreate(10, sizeof(int));
    xEthernetQueue = xQueueCreate(10, sizeof(int));
    xRS232Queue = xQueueCreate(10, sizeof(int));

    if (xQueue1 == NULL)
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    // A
    xLastWakeTime = xTaskGetTickCount();

    // B
    for (;;)
    {
        // C
        vTaskDelayUntil(&xLastWakeTime, CYCLE_RATE_MS);

        // Request data from the sensors.
        Data.Data1 = getSensorValue1();
        Data.Data2 = getSensorValue2();

        // D
        if (xQueueReceive(xFieldBusQueue, &sendData1, MAX_COMMS_DELAY) == pdTRUE)
        {
            // E
            if (xQueueReceive(xFieldBusQueue, &sendData2, MAX_COMMS_DELAY) == pdTRUE)
            {
                //TransmitResults();
                xQueueSend(xQueue1, &Data.Data1, MAX_COMMS_DELAY);
                xQueueSend(xQueue1, &Data.Data2, MAX_COMMS_DELAY);

                //printf("sensor 1: %d, sensor 2: %d\n", Data.Data1, Data.Data2);
            }
        }
        else
        {
            printf("Sensor Queue empty\n");
        }
    }

    // Will never get here!
}

int getSensorValue1(void)
{
    srand((unsigned)time(NULL));

    int number = rand() % (maxValue1 - minValue1 + 1) + maxValue1;

    if (xFieldBusQueue == NULL)
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    xQueueSend(xFieldBusQueue, &number, MAX_COMMS_DELAY);
    xQueueSend(xEthernetQueue, &number, MAX_COMMS_DELAY);
    xQueueSend(xRS232Queue, &number, MAX_COMMS_DELAY);

    return number;
}

int getSensorValue2(void)
{
    srand((unsigned)time(NULL));

    int number = rand() % (maxValue2 - minValue2 + 1) + minValue2;

    if (xFieldBusQueue == NULL)
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    xQueueSend(xFieldBusQueue, &number, MAX_COMMS_DELAY);
    xQueueSend(xEthernetQueue, &number, MAX_COMMS_DELAY);
    xQueueSend(xRS232Queue, &number, MAX_COMMS_DELAY);

    return number;
}

/*
-----------------------------------------------------------------------------------------
*/

/* Web Server Task */
void WebServerTask(void *pvParameters)
{
    int Data;

    for (;;)
    {
        // Block until data arrives.  xEthernetQueue is filled by the
        // Ethernet interrupt service routine.
        if (xQueueReceive(xEthernetQueue, &Data, portMAX_DELAY) == pdTRUE)
        {
            ProcessHTTPData(Data);
        }
    }
}

void ProcessHTTPData(int Data)
{
    if(flagWeb == 1){
        printf("\nData Received from Web Server --> latest sensor data = %d \n", Data);
        flagWeb = 0;
    }
}

/*
-----------------------------------------------------------------------------------------
*/

/* RS232 Task */
void RS232Task(void *pvParameters)
{
    int Data, count;

    count = 0;

    for (;;)
    {
        // Block until data arrives.  xRS232Queue is filled by the
        // RS232 interrupt service routine.
        if (xQueueReceive(xRS232Queue, &Data, portMAX_DELAY) == pdTRUE)
        {
            ProcessSerialCharacters(Data);
        }
    }
}

void ProcessSerialCharacters(int Data)
{
    if(flagPDA == 1){
        printf("\nPDA Received Data --> latest sensor data = %d\n", Data);
        flagPDA = 0;
    }
}

/*
-----------------------------------------------------------------------------------------
*/

/* Keypad Scan Task */
void KeyScanTask(void *pvParmeters)
{
    char Key;
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, DELAY_PERIOD_KP);

        UpdateDisplay(Key);
        // Scan the keyboard.
        if (kbhit())
        {
            fflush(stdout);

            Key = getchar();

            switch (Key)
            {
            case '1':
                ecoMode();
                break;
            case '2':
                defaultMode();
                break;
            case '3':
                turboMode();
                break;
            case '4':
                UpdateDisplay(Key);
                break;
            case '5':
                flagPDA = 1;
                break;
            case '6':
                flagWeb = 1;
                break;
            default:
                break;
            }
        }
    }
}

void UpdateDisplay(char Key)
{
    
    if(Key == '4')
        printf("\ncpu usage: %.2f% \n", cpuMeansure * 100);
}


void ecoMode()
{
    int maxValue, minValue;
    minValue = 0;
    maxValue = 1250;

    printf("\n---- engine switch to ECO mode ----\n");
    maxValue1 = maxValue;
    minValue1 = minValue;
    maxValue2 = maxValue+2050;
    minValue2 = minValue+1025;
}

void defaultMode()
{
    int maxValue, minValue;
    minValue = 1001;
    maxValue = 5000;

    printf("\n---- engine switch to DEFAULT mode ----\n");
    maxValue1 = maxValue;
    minValue1 = minValue;
    maxValue2 = maxValue+2050;
    minValue2 = minValue+1025;
}

void turboMode()
{
    int maxValue, minValue;
    minValue = 5001;
    maxValue = 11000;

    printf("\n---- engine switch to Turbo mode ----\n");
    maxValue1 = maxValue;
    minValue1 = minValue;
    maxValue2 = maxValue+2050;
    minValue2 = minValue+1025;
}

/*
-----------------------------------------------------------------------------------------
*/

/* LED Task */
void LEDTask(void *pvParmeters)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        //Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, DELAY_PERIOD);

        // Flash the appropriate LED.
        if (SystemIsHealthy() == 0)
        {
            FlashLED(GREEN, statusLED);
        }
        else
        {
            FlashLED(RED, statusLED);
        }
    }
}
int SystemIsHealthy(void)
{
    /*
    verifica se houve erro de perca de deadline ou sobrecarga de cpu
        estiver tudo ok, retorna 0. Se não, retorna 1
    */
    return systemHealth;
}
int FlashLED(int led, int status)
{
    const TickType_t xPeriod = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        if (status == 1 && led == 0)
        {
            printf("\tLED: GREEN \n \t\t---> HIGH\r\n");
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            status = 0;
        }
        else if (status == 0 && led == 0)
        {
            printf("\tLED: GREEN \n \t\t---> LOW\r\n");
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            status = 1;
        }
        else if (status == 1 && led == 1)
        {
            printf("\tLED: RED \n \t\t---> HIGH\r\n");
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            status = 0;
        }
        else if (status == 0 && led == 1)
        {
            printf("\tLED: RED \n \t\t---> LOW\r\n");
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            status = 1;
        }
    }
}
/*
-----------------------------------------------------------------------------------------
*/

/*
    CPU Usage
*/
void vApplicationIdleHook(void)
{
    TickType_t curr = xTaskGetTickCount();
    idleTask += xTaskGetTickCount() - curr;
}

void cpuUsage(void *pvParmeters)
{
    cpuMeansure = 1 - ((float)idleTask / (float)xTaskGetTickCount());
    //printf("|cpu usage: %.2f% |", cpuUsage * 100);
    
    if (cpuMeansure > 0.85)
    {
        systemHealth = 1;
    }
    else if (cpuMeansure <= 0.85)
    {
        systemHealth = 0;
    }
}
/*
-----------------------------------------------------------------------------------------
*/

void initMenu( void ){
    printf( "Hello from Freertos\r\n" );
    printf("\n-------------------------------------\n");
    printf("\n Menu de opções do sistema\r\n");
    printf("\n opc = 1 --> Motor modo econômico\r\n");
    printf("\n opc = 2 --> Motor modo padrão\r\n");
    printf("\n opc = 3 --> Motor modo turbo\r\n");
    printf("\n opc = 4 --> Visualizar carga da CPU\r\n");
    printf("\n opc = 5 --> Acesso a PDA interface\r\n");
    printf("\n opc = 6 --> Acesso a Web interface\r\n");
    printf("\n-------------------------------------\n");
    printf("\nO sistema iniciara em alguns segundos!\n\n");
}