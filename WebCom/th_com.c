#include "com/com.h"
#include "sd/sd.h"
#include "rtc.h"
#include <cstdlib>

osThreadId_t thweb_comTx;
osThreadId_t thweb_comRx;

extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;

static uint8_t int_taq = 0;

void th_webcom_Tx(void *argument);                   
void th_webcom_Rx(void *argument);

int init_thcomweb (void) {
 
  thweb_comTx = osThreadNew(th_webcom_Tx, NULL, NULL);
	thweb_comRx = osThreadNew(th_webcom_Rx, NULL, NULL);

  if (thweb_comTx == NULL || thweb_comRx == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_webcom_Tx (void *argument) {
 ComData_t msg;
 
  while (1) {
    osMessageQueueGet(qCom_Rx,&msg,NULL,osWaitForever);
    
    switch(msg.cmd){
      case HORA:
        msg.cmd = RESP_HORA;
        msg.length = sprintf(msg.buff, "%s %s", fecha_rec.BufHour, fecha_rec.BufDate);
        osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
      
      break;
            
      case ALARM_COM:
        osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      case LECTURA_NFC:
        msg.cmd = RESP_ESTADO_TAQUILLA;
        int_taq = nfc_search(msg.buff);
        if (int_taq == 1){
          // Alterna el bit 1 (valor decimal 1, binario 00000001)
          estado_taq ^= (1 << 0);
          if ((estado_taq & 0x01) == 0){
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha CERRADO la taquilla 1");
          }else{
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha ABIERTO la taquilla 1");
          }
          
        }else if (int_taq == 2){
          // Alterna el bit 2 (valor decimal 2, binario 00000010)
          estado_taq ^= (1 << 1);
          if ((estado_taq & 0x02) == 0){
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha CERRADO la taquilla 2");
          }else{
             alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha ABIERTO la taquilla 2");
           }
        }
        osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      case RESP_DORMIR:
        osMessageQueueGet(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      case RESP_ALARM_COM:
        osMessageQueueGet(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      case RESP_LECTURA_PESO:
        msg.buff;
      break;
      
      case RESP_LECTURA_TENSION:
        tens = (float)atof(msg.buff);
      break;
      
      case RESP_LECTURA_CORRIENTE:
        osMessageQueueGet(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      
    }

    osThreadYield();
  }
}

void th_webcom_Rx(void *argument){

  while(1){

    osThreadYield();

  }
}
