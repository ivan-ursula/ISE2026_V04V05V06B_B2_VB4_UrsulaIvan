#include "cmsis_os2.h"     

#include "prueba.h"

estados_t estado;
osThreadId_t thmain;                       
extern osMessageQueueId_t q_nfc;

msg_nfc uid;
void th_main (void *argument);                   
 
int Init_main (void) {
 
  thmain = osThreadNew(th_main, NULL, NULL);
  if (thmain == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_main (void *argument) {
  estado=ACTIVO;
  while (1) {
		
		switch(estado){
			case ACTIVO:
				
				break ;
			case BAJO_CONSUMO:
				
				break;
			case ALARMA:
				
			
				break;
			
			
			
		}
		
		
    osMessageQueueGet(q_nfc,&uid,0,osWaitForever);
    osThreadYield();                           
  }
}
