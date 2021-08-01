/* Standard includes. */
#include <unistd.h>
#include <stdarg.h>

#include "controller.h"

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;
TaskHandle_t xHandle5 = NULL;
TaskHandle_t xHandle6 = NULL;

int main( void ){
	printf( "Hello from Freertos\r\n" );

	char *c1 = "leds";
	char *c2 = "keypad";
	char *c3 = "plant control task";
	char *c4 = "web task";
	char *c5 = "RS232 task";
	char *c6 = "cpu usage";

	vSchedulerInit();

	vSchedulerPeriodicTaskCreate(LEDTask, "Task LEDs", configMINIMAL_STACK_SIZE, &c1, 0, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(50), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(KeyScanTask, "Task KeyPad", configMINIMAL_STACK_SIZE, &c2, 1, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(15), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(PlantControlTask, "Plant Task", configMINIMAL_STACK_SIZE, &c3, 2, &xHandle3, pdMS_TO_TICKS(0), pdMS_TO_TICKS(10), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(WebServerTask, "Web Server Task", configMINIMAL_STACK_SIZE, &c4, 0, &xHandle4, pdMS_TO_TICKS(0), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(RS232Task, "PDA Task", configMINIMAL_STACK_SIZE, &c5, 1, &xHandle5, pdMS_TO_TICKS(10), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	vSchedulerPeriodicTaskCreate(cpuUsage, "CPU Task", configMINIMAL_STACK_SIZE, &c6, 1, &xHandle6, pdMS_TO_TICKS(10), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(100), pdMS_TO_TICKS(1000));
	
	
	vSchedulerStart();
	
	for( ;; );

	

	return 0;
}

