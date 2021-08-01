#include "controller.h"
#include "kb.h"

int statusLED = 1;
int systemHealth = 0;

QueueHandle_t xQueue1;
QueueHandle_t xFieldBusQueue;
QueueHandle_t xEthernetQueue;
QueueHandle_t xRS232Queue;

TickType_t idleTask = 0;

/* Plant Control Task */
void PlantControlTask( void *pvParameters ){
    TickType_t xLastWakeTime;
    int Data1, Data2 ,sendData1, sendData2;


    /* Queue init */
    xQueue1 = xQueueCreate( 10, sizeof( int ) );
    xFieldBusQueue = xQueueCreate( 10, sizeof( int ) );
    xEthernetQueue = xQueueCreate( 10, sizeof( int ) );
    xRS232Queue = xQueueCreate( 10, sizeof( int ) );


    if( xQueue1 == NULL )
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    // A
    xLastWakeTime = xTaskGetTickCount();

    // B
    for( ;; )
    {
        // C
        vTaskDelayUntil( &xLastWakeTime, CYCLE_RATE_MS );
        
        // Request data from the sensors.
        Data1 = getSensorValue1();
        Data2 = getSensorValue1();
        
        // D
        if( xQueueReceive( xFieldBusQueue, &sendData1, MAX_COMMS_DELAY ) == pdTRUE )
        {
            // E
            if( xQueueReceive( xFieldBusQueue, &sendData2, MAX_COMMS_DELAY ) == pdTRUE )
            {
                //TransmitResults();   
                xQueueSend( xQueue1, &Data1, MAX_COMMS_DELAY );
                xQueueSend( xQueue1, &Data2, MAX_COMMS_DELAY ); 
                //printf("sensor 1: %d, sensor 2: %d\n", Data1, Data2);        
            }
        } else {
            printf("Sensor Queue empty\n");
        } 
        
    }
    
    // Will never get here!
}

int getSensorValue1( void ){
    srand( (unsigned)time(NULL) );

    int number = rand() % (5000 - 0 + 1) + 0;

    if( xFieldBusQueue == NULL )
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    
    xQueueSend( xFieldBusQueue, &number, MAX_COMMS_DELAY );
    xQueueSend( xEthernetQueue, &number, MAX_COMMS_DELAY );
    xQueueSend( xRS232Queue, &number, MAX_COMMS_DELAY );

    return number;
}

int getSensorValue2( void ){
    srand( (unsigned)time(NULL) );

    int number = rand() % (7000 - 0 + 1) + 0;


    if( xFieldBusQueue == NULL )
    {
        /* Queue was not created and must not be used. */
        printf("Queue create error\n");
    }

    
    xQueueSend( xFieldBusQueue, &number, MAX_COMMS_DELAY );
    xQueueSend( xEthernetQueue, &number, MAX_COMMS_DELAY );
    xQueueSend( xRS232Queue, &number, MAX_COMMS_DELAY );

    return number;
}

/*
-----------------------------------------------------------------------------------------
*/

/* Web Server Task */
void WebServerTask( void *pvParameters ){
    int Data;

    for( ;; )
    {
        // Block until data arrives.  xEthernetQueue is filled by the
        // Ethernet interrupt service routine.
        if( xQueueReceive( xEthernetQueue, &Data, portMAX_DELAY ) == pdTRUE )
        {
            ProcessHTTPData( Data );
        }        
    }
}

void ProcessHTTPData( int Data ){
    //printf("Data Received from Web Server -> %d\n", Data);
}

/*
-----------------------------------------------------------------------------------------
*/

/* RS232 Task */
void RS232Task( void *pvParameters ){
    int Data;

    for( ;; )
    {
        // Block until data arrives.  xRS232Queue is filled by the
        // RS232 interrupt service routine.
        if( xQueueReceive( xRS232Queue, &Data, portMAX_DELAY ) == pdTRUE )
        {
            ProcessSerialCharacters( Data );
        }        
    }
}

void ProcessSerialCharacters( int Data ){
    //printf("PDA Received Data --> %d\n", Data);
}

/*
-----------------------------------------------------------------------------------------
*/

/* Keypad Scan Task */
void KeyScanTask( void *pvParmeters ){
    char Key;
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, DELAY_PERIOD_KP );
        
        // Scan the keyboard.
        if(kbhit()){
            fflush(stdout);

            Key = getchar();
            UpdateDisplay( Key );
        }
    }
}

void UpdateDisplay( char Key ){
    printf("\nChar received:%c\n", Key);
    printf("Done.\n");
}

void selection(int temp){
    int selectData;

    for( ;; )
    {
        xQueueReceive( xFieldBusQueue, &selectData, MAX_COMMS_DELAY );
        if(selectData == temp){
            printf("Found temperature: %d °C.\n", selectData);
            break;
        }else{
            printf("Temperature not found.\n");
            break;
        }
    }
    
}

void visualization(QueueHandle_t xQueue){
    int Data;
    printf("------------------------------"); 
    for( ;; ){
        xQueueReceive( xQueue, &Data, MAX_COMMS_DELAY ); 
        printf("Temperature : %d ºC.\n", Data);  
    }
    printf("------------------------------"); 
}

void modification(QueueHandle_t xQueue, int temp){
    xQueueSend( xQueue, &temp, MAX_COMMS_DELAY );
}


/*
-----------------------------------------------------------------------------------------
*/

/* LED Task */
void LEDTask( void *pvParmeters ){
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    for( ;; ){
        //Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, DELAY_PERIOD );

        // Flash the appropriate LED.
        if( SystemIsHealthy() == GREEN ){
            FlashLED( GREEN, statusLED );

        }
        else {
            FlashLED( RED, statusLED );
        }
    }
}
int SystemIsHealthy( void ){
    /*
    * TODO: verifica se houve erro de perca de deadline ou sobrecarga de cpu
    *    estiver tudo ok, retorna 0. Se não, retorna 1
    */
   return systemHealth;
}
int FlashLED( int led, int status ){
    const TickType_t xPeriod = pdMS_TO_TICKS( 1000 ) ;
    TickType_t xLastWakeTime;

    if( led == GREEN ){
        printf("\r*** LED GREEN ****\n");
    }
    else {
        printf("\r**** LED RED ****\n");
    }

    xLastWakeTime = xTaskGetTickCount();
    for( ;; ){
        if( status == 1 ) {
            printf("\tstatus LED ---> HIGH\n");
            vTaskDelayUntil( &xLastWakeTime, xPeriod );
            status = 0;
        } else if( status == 0 ){
            printf("\tstatus LED ---> LOW\n");
            vTaskDelayUntil( &xLastWakeTime, xPeriod );
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
void vApplicationIdleHook( void ){
    TickType_t curr = xTaskGetTickCount();
    idleTask += xTaskGetTickCount() - curr;
}

void cpuUsage( void *pvParmeters ){
    float cpuUsage = 1 - ((float) idleTask / (float) xTaskGetTickCount());
    printf("cpu usage: %.2f\n", cpuUsage);
    if(cpuUsage > 0.85){
        systemHealth = 1;
    } 
    else if(cpuUsage <= 0.85){
        systemHealth = 0;
    }
}
/*
-----------------------------------------------------------------------------------------
*/