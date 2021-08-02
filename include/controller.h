#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Plant Control  */
#define CYCLE_RATE_MS           10
#define MAX_COMMS_DELAY         5
//#define MAX_COMMS_DELAY         2

/* Keypad Scan*/
//#define DELAY_PERIOD_KP         4
#define DELAY_PERIOD_KP         50

/* LED  */
#define DELAY_PERIOD            1000
#define GREEN                   0
#define RED                     1

/* Web Server */
#define HTTP_REQUEST_PROC_LOAD  5

/* PDA */
#define RS232_CHAR_PROC_LOAD    5

#include "scheduler.h"
#include <stdbool.h>
#include <time.h>
#include "queue.h"
//#include <string.h>

/* Plant Control Task */
void PlantControlTask( void *pvParameters );
void getSensorValue1( void );
void getSensorValue2( void );  

/* Web Server Task */
void WebServerTask( void *pvParameters );
void ProcessHTTPData( int Data, int Data2 );
/* RS232 Task */
void RS232Task( void *pvParameters );
void ProcessSerialCharacters( int Data, int Data2 );
/* Keypad Scan Task */
void KeyScanTask( void *pvParmeters );
void UpdateDisplay( char Key );

void ecoMode( void );
void defaultMode( void );
void turboMode( void );
/* LED Task */
void LEDTask( void *pvParmeters );
int SystemIsHealthy( void );
int FlashLED( int led, int status );
/* CPU usage */
void vApplicationIdleHook( void );
void cpuUsage( void *pvParmeters );

void initMenu( void );


#endif /* CONTROLLER_H */