#include "com/com.h"
#include "sd/sd.h"
#include "rtc.h"
#include <cstdlib>

osThreadId_t thweb_comTx;
osThreadId_t thweb_comRx;

extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;

extern osMessageQueueId_t mid_Msg_Taq;
MSGQUEUE_OBJ_DATE fecha_rec_taq;

static uint8_t flag = 0x01;
static uint8_t flag_alarm = 0x02;

static uint8_t int_taq = 0;
static char mensaje[75];

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
 
void th_webcom_Rx (void *argument) {
 ComData_t msg_rx;
 
  while (1) {
    osMessageQueueGet(qCom_Rx,&msg_rx,NULL,osWaitForever);
		osMessageQueueGet(mid_Msg_Taq, &fecha_rec_taq, NULL, 0);
    
    switch(msg_rx.cmd){
      case HORA:
        osDelay(50);
        msg_rx.cmd = RESP_HORA;
        msg_rx.length = sprintf(msg_rx.buff, "%s %s", fecha_rec_taq.BufHour, fecha_rec_taq.BufDate);
        osMessageQueuePut(qCom_Tx,&msg_rx,NULL,osWaitForever);
      
      break;
            
      case ALARM_COM:
        alerta = 1;
        modo_func = 0x02;
        alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha detectado una presencia en horas no activas");
        flag_alarm = osThreadFlagsWait(0x06, osFlagsWaitAny, osWaitForever);
        
        msg_rx.cmd = RESP_ALARM_COM;
        if(flag_alarm == 0x02){
          msg_rx.length = 1;
          msg_rx.buff[0] = 0x01;
        }else if(flag_alarm == 0x04){
          msg_rx.length = 0;
          msg_rx.buff[0] = 0x00;
        }
        osMessageQueuePut(qCom_Tx,&msg_rx,NULL,osWaitForever);
      break;
      
      case LECTURA_NFC:
        msg_rx.cmd = RESP_ESTADO_TAQUILLA;
        int_taq = nfc_search(msg_rx.buff);
        if (int_taq == 1){
          // Alterna el bit 1 (valor decimal 1, binario 00000001)
          estado_taq ^= (1 << 0);
          if ((estado_taq & 0x01) == 0){
						sprintf(mensaje, "Se ha CERRADO la taquilla 1: %s", msg_rx.buff);
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
          }else{
						sprintf(mensaje, "Se ha ABIERTO la taquilla 1: %s", msg_rx.buff);
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
          }
          
        }else if (int_taq == 2){
          // Alterna el bit 2 (valor decimal 2, binario 00000010)
          estado_taq ^= (1 << 1);
          if ((estado_taq & 0x02) == 0){
						sprintf(mensaje, "Se ha CERRADO la taquilla 2: %s", msg_rx.buff);
            alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
          }else{
						 sprintf(mensaje, "Se ha ABIERTO la taquilla 2: %s", msg_rx.buff);
             alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
           }
        }
				
        strcpy(id_nfc_new, msg_rx.buff);

        msg_rx.length = sprintf(msg_rx.buff, "%d", estado_taq);
        osMessageQueuePut(qCom_Tx,&msg_rx,NULL,osWaitForever);
      break;
      
      case RESP_DORMIR:
        modo_func = 0x02;
        sprintf(mensaje, "Se ha dormido: %02d:%02d",hora_dorm_per, min_dorm_per);
        alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
      break;
      
      case RESP_DESPERTAR:
        modo_func = 0x01;
        sprintf(mensaje, "Se ha despertado: %02d:%02d",hora_desp_per, min_desp_per);
        alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
      break;
      
      case RESP_LECTURA_PESO:
        if (peso_ini1 == 0 && peso_ini2 == 0){
          sscanf(msg_rx.buff, "%f-%f", &peso_ini2, &peso_ini1);
        }else {
          sscanf(msg_rx.buff, "%f-%f", &peso_taq1, &peso_taq2);
        }
          
      break;
      
      case RESP_LECTURA_TENSION:
        tens = (float)atof(msg_rx.buff);
      break;
      
      case RESP_LECTURA_CORRIENTE:
        intens = (float)atof(msg_rx.buff);
      break;
      
      
    }

    osThreadYield();
  }
}

void th_webcom_Tx(void *argument){
  ComData_t msg_tx;
  
  while(1){
    flag = osThreadFlagsWait(0x0F, osFlagsWaitAny, osWaitForever);
    if(modo_func == 1){
			if (flag == 0x01){ //Se guarda la hora y se envía directamente
        osDelay(50);
				msg_tx.cmd = DORMIR;
				msg_tx.length = sprintf(msg_tx.buff, "%02d:%02d",hora_dorm_per, min_dorm_per);
				sprintf(mensaje, "Se dormirá: %02d:%02d",hora_dorm_per, min_dorm_per);
				alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
				osMessageQueuePut(qCom_Tx,&msg_tx,NULL,osWaitForever);
				
			}else if (flag == 0x02){ 
        osDelay(50);
				msg_tx.cmd = DESPERTAR;
				msg_tx.length = sprintf(msg_tx.buff, "%02d:%02d", hora_desp_per, min_desp_per);
				sprintf(mensaje, "Se despertará: %02d:%02d ", hora_desp_per, min_desp_per);
				alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
				osMessageQueuePut(qCom_Tx,&msg_tx,NULL,osWaitForever);
        
      } else {
        osDelay(50);
				msg_tx.cmd = LECTURA_PESO;
				msg_tx.length = 0;
				msg_tx.buff[0] = 0x00;
				osMessageQueuePut(qCom_Tx,&msg_tx,NULL,osWaitForever);
				
        osDelay(50);
				msg_tx.cmd = LECTURA_TENSION;
				msg_tx.length = 0;
				msg_tx.buff[0] = 0x00;
				osMessageQueuePut(qCom_Tx,&msg_tx,NULL,osWaitForever);

				osDelay(50);
				msg_tx.cmd = LECTURA_CORRIENTE;
				msg_tx.length = 0;
				msg_tx.buff[0] = 0x00;
				osMessageQueuePut(qCom_Tx,&msg_tx,NULL,osWaitForever);
			}
		}
		flag = 0;
    
    osThreadYield();
  }
}
