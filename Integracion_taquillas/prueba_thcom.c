#include "cmsis_os2.h"     

#include "prueba.h"


osThreadId_t thmain;                       
extern osMessageQueueId_t q_nfc;

extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;

estados_t estado;
msg_nfc uid;
ComData_t msg_tx;
ComData_t msg_rx;
uint8_t status_q;

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
				status_q=osMessageQueueGet(q_nfc,&uid,0,300);
			
				if(status_q==osOK){
					msg_tx.cmd= 20; //cmd de uid 
					msg_tx.length=uid.length;
					for(int i=0;i<uid.length;i++){
						msg_tx.buff[i]=uid.buff[i];
						
					}
	
			
					osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
				
					
				}	
			//	osMessageQueueGet(qCom_Rx,&msg_uart,0,20);
				
				
				break ;
			case BAJO_CONSUMO:
				
				break;
			case ALARMA:
				
			
				break;			
			
		}
		
		
    
                       
  }
}
