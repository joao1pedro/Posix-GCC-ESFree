#include "controller.h"
#include "kb.h"

/* Plant Control Task */
void PlantControlTask( void *pvParameters ){

}

/*
-----------------------------------------------------------------------------------------
*/

/* Web Server Task */
void WebServerTask( void *pvParameters ){

}

/*
-----------------------------------------------------------------------------------------
*/

/* RS232 Task */
void RS232Task( void *pvParameters ){

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
            FlashLED( GREEN );

        }
        else {
            FlashLED( RED );
        }
    }
}
int SystemIsHealthy( void ){
    /*
    * TODO: verifica se houve erro de perca de deadline ou sobrecarga de cpu
    *    estiver tudo ok, retorna 0. Se não, retorna 1
    */
   return 0;
}
int FlashLED( int led ){
    TickType_t xLastWakeTime;
    //const TickType_t xFrequency = 1000;

    if( led == GREEN ){
        printf("\r*** LED GREEN ****\n");
    }
    else {
        printf("\r**** LED RED ****\n");
    }

    xLastWakeTime = xTaskGetTickCount();
    for( ;; ){
        printf("\tstatus LED ---> LOW\n");
        vTaskDelayUntil( &xLastWakeTime, 1000/pdMS_TO_TICKS(50) );
        printf("\tstatus LED ---> HIGH\n");
    }
}
/*
-----------------------------------------------------------------------------------------
*/