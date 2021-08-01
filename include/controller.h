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
#include <time.h>
#include "queue.h"
//#include <string.h>

/* Plant Control Task */
void PlantControlTask( void *pvParameters );
int getSensorValue1( void );
int getSensorValue2( void );  
/* Web Server Task */
void WebServerTask( void *pvParameters );
void ProcessHTTPData( int Data );
/* RS232 Task */
void RS232Task( void *pvParameters );
void ProcessSerialCharacters( int Data );
/* Keypad Scan Task */
void KeyScanTask( void *pvParmeters );
void UpdateDisplay( char Key );
/* LED Task */
void LEDTask( void *pvParmeters );
int SystemIsHealthy( void );
int FlashLED( int led, int status );


#endif /* CONTROLLER_H */