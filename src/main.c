/* Standard includes. */
#include <unistd.h>
#include <stdarg.h>

#include "controller.h"

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;

int main( void ){
	printf( "Hello from Freertos\r\n" );

	char *c1 = "leds";
	char *c2 = "keypad";

	vSchedulerInit();

	vSchedulerPeriodicTaskCreate(LEDTask, "Task LEDs", configMINIMAL_STACK_SIZE, &c1, 0, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(55), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(KeyScanTask, "Task KeyPad", configMINIMAL_STACK_SIZE, &c2, 1, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(15), pdMS_TO_TICKS(20), pdMS_TO_TICKS(1000));
	
	vSchedulerStart();
	
	for( ;; );

	

	return 0;
}

