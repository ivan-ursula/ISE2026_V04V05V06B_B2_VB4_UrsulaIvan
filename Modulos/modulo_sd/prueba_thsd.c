#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "sd.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t th_id_prueba_sd;                        // thread id
void th_prueba_SD (void *argument);                   // thread function
 
int init_th_prueba_com (void) {
 
  th_id_prueba_sd = osThreadNew(th_prueba_SD, NULL, NULL);
  if (th_id_prueba_sd == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_prueba_SD (void *argument) 
{
	while (1) 
		{
		
		}
		osThreadYield();                            // suspend thread
}
