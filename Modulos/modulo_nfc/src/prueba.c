#include "cmsis_os2.h"                          
#include "NFC.h"
 
osThreadId_t tid_Thread;                       
extern osMessageQueueId_t q_nfc;

msg_nfc uid;
void Thread (void *argument);                   
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  while (1) {
    osMessageQueueGet(q_nfc,&uid,0,osWaitForever);
    osThreadYield();                           
  }
}
