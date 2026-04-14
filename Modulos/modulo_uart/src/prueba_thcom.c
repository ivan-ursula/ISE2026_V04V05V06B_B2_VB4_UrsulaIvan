#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "com.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t th_prueba_com;                        // thread id
extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;
void th_prueba_N (void *argument);                   // thread function
 
int init_th_prueba_com (void) {
 
  th_prueba_com = osThreadNew(th_prueba_N, NULL, NULL);
  if (th_prueba_com == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_prueba_N (void *argument) {
 ComData_t msg;
  while (1) {
    
    osMessageQueueGet(qCom_Rx,&msg,0,osWaitForever);
		float d= 0.23;
		msg.cmd=RESP_HORA;
		msg.length=sprintf(msg.buff,"%.2f",d);
    osMessageQueuePut(qCom_Tx,&msg,0,0);
     // Insert thread code here...
		//osDelay(1000);
    osThreadYield();                            // suspend thread
  }
}
