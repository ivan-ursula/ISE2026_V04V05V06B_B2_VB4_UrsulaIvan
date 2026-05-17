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
 ComData_t msg;
 
  while (1) {
    osMessageQueueGet(qCom_Rx,&msg,NULL,osWaitForever);
		osMessageQueueGet(mid_Msg_Taq, &fecha_rec_taq, NULL, 0);
    
    switch(msg.cmd){
      case HORA:
        msg.cmd = RESP_HORA;
        msg.length = sprintf(msg.buff, "%s %s", fecha_rec_taq.BufHour, fecha_rec_taq.BufDate);
        osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
      
      break;
            
      case ALARM_COM:
        alerta = 1;
        modo_func = 0x02;
        alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, "Se ha detectado una presencia en horas no activas");
        flag_alarm = osThreadFlagsWait(0x06, osFlagsWaitAny, osWaitForever);
        
        msg.cmd = RESP_ALARM_COM;
        if(flag_alarm == 0x02){
          msg.length = 1;
          msg.buff[0] = 0x01;
        }else if(flag_alarm == 0x04){
          msg.length = 0;
          msg.buff[0] = 0x00;
        }
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
        msg.length = sprintf(msg.buff, "%d", estado_taq);
        osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
      break;
      
      case RESP_DORMIR:
        modo_func = 0x02;
        sprintf(mensaje, "Se ha dormido - Se despertará: %02d:%02d %s",hora_desp, min_desp, fecha_desp);
        alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
      break;
      
      case RESP_LECTURA_PESO:
        sscanf(msg.buff, "%f-%f", &peso_taq1, &peso_taq2);
      break;
      
      case RESP_LECTURA_TENSION:
        tens = (float)atof(msg.buff);
      break;
      
      case RESP_LECTURA_CORRIENTE:
        intens = (float)atof(msg.buff);
      break;
      
      
    }

    osThreadYield();
  }
}

void th_webcom_Tx(void *argument){
  ComData_t msg;
	int anio = 0;
	int mes = 0;
  int dia = 0;
	char fecha_dorm_taq[12];
	char fecha_desp_taq[12];
  
  while(1){
    flag = osThreadFlagsWait(0x0F, osFlagsWaitAny, osWaitForever);
    
		if (flag == 0x01){ //Se guarda la hora y se envía directamente
			msg.cmd = DORMIR;
			if (sscanf(fecha_dorm, "%d-%d-%d", &anio, &mes, &dia) == 3) {
        sprintf(fecha_dorm_taq, "%02d/%02d/%04d\n", dia, mes, anio);
			}
			if (sscanf(fecha_desp, "%d-%d-%d", &anio, &mes, &dia) == 3) {
        sprintf(fecha_desp_taq, "%02d/%02d/%04d\n", dia, mes, anio);
			}
			
			msg.length = sprintf(msg.buff, "%02d:%02d:00 %s-%02d:%02d:00 %s",hora_dorm, min_dorm, fecha_dorm_taq, hora_desp, min_desp, fecha_desp_taq);
			sprintf(mensaje, "Se dormirá: %02d:%02d %s - Se despertará: %02d:%02d %s",hora_dorm, min_dorm, fecha_dorm, hora_desp, min_desp, fecha_desp);
			alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
			osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
			osDelay(5);
			
		}else if (flag == 0x03){ //Se envía la fecha actual para dormir
			msg.cmd = DORMIR;
			osMessageQueueGet(mid_Msg_Taq, &fecha_rec_taq, NULL, 10);
			if (sscanf(fecha_desp, "%d-%d-%d", &anio, &mes, &dia) == 3) {
        sprintf(fecha_desp_taq, "%02d/%02d/%04d\n", dia, mes, anio);
			}
			
			msg.length = sprintf(msg.buff, "%s %s-%02d:%02d:00 %s",fecha_rec_taq.BufHour, fecha_rec_taq.BufDate, hora_desp, min_desp, fecha_desp_taq);
			sprintf(mensaje, "Se duerme - Se despertará: %02d:%02d %s", hora_desp, min_desp, fecha_desp);
			alarm_write(fecha_rec.BufDate, fecha_rec.BufHour, mensaje);
			osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
			osDelay(5);
			
		}else{ 
			msg.cmd = LECTURA_PESO;
			msg.length = 0;
			msg.buff[0] = 0x00;
			osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
			osDelay(5);
			
			msg.cmd = LECTURA_TENSION;
			msg.length = 0;
			msg.buff[0] = 0x00;
			osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
			osDelay(5);
			
			msg.cmd = LECTURA_CORRIENTE;
			msg.length = 0;
			msg.buff[0] = 0x00;
			osMessageQueuePut(qCom_Tx,&msg,NULL,osWaitForever);
			osDelay(5);
		}
		
		flag = 0;
    
    osThreadYield();
  }
}
