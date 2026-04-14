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
 ComData_t msg_rx;
 ComData_t msg_tx;
  while (1) {
    
    osMessageQueueGet(qCom_Rx,&msg_rx,0,osWaitForever);
		if(msg_rx.cmd == HORA){
			msg_tx.cmd = RESP_HORA;
		char *d= "Hola, soy el receptor";
		msg_tx.length=sprintf(msg_tx.buff,"%s",d);
    osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
     // Insert thread code here...
		//osDelay(1000);
		}
    osThreadYield();                            // suspend thread
  }
}
