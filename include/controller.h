#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Plant Control  */
#define CYCLE_RATE_MS           10
#define MAX_COMMS_DELAY         2

/* Keypad Scan*/
//#define DELAY_PERIOD_KP         4
#define DELAY_PERIOD_KP         50

/* LED  */
#define DELAY_PERIOD            1000
#define GREEN                   0
#define RED                     1

#include "scheduler.h"
#include <stdbool.h>
//#include <string.h>

/* Plant Control Task */
void PlantControlTask( void *pvParameters );
/* Web Server Task */
void WebServerTask( void *pvParameters );
/* RS232 Task */
void RS232Task( void *pvParameters );
/* Keypad Scan Task */
void KeyScanTask( void *pvParmeters );
void UpdateDisplay( char Key );
/* LED Task */
void LEDTask( void *pvParmeters );
int SystemIsHealthy( void );
int FlashLED( int led );



#endif /* CONTROLLER_H */