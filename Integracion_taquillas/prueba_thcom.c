#include "cmsis_os2.h"     
#include "stm32f4xx_hal.h"
#include "prueba.h"
#include "RTC.h"
#include <time.h>

//ID DE HILOS
osThreadId_t thmain;  
//ID DE COLAS DE MENSAJES
extern osMessageQueueId_t q_nfc;
extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;

//VARIABLES DE COLAS DE MENSAJES
msg_nfc uid;
ComData_t msg_tx;
ComData_t msg_rx;
uint8_t status_q;

//TIMER HILO PRINCIPAL
osTimerId_t tim_RTC;

//VARIABLES GILO PRINCIPAL
estados_t estado;
RTC_AlarmTypeDef alarma;
uint32_t exec2;
volatile uint8_t flag_tim_rtc=0;

void th_main (void *argument);                   
 
int Init_main (void) {
 
  thmain = osThreadNew(th_main, NULL, NULL);
	tim_RTC=osTimerNew((osTimerFunc_t)&TimerRTC_Callback, osTimerPeriodic, &exec2, NULL);
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
				
				estado_Activo();


				
				break ;
			case BAJO_CONSUMO:
				
				break;
			case ALARMA:
				
			
				break;			
			
		}
		
		
    
                       
  }
}

void estado_Activo(void){
	status_q=osMessageQueueGet(qCom_Rx,&msg_rx,0,20);
	if(status_q==osOK){
		comp_cmd(msg_rx);
	}			

	//SI QUIERES PROBAR SIN NFC PUEDE QUE TE DE EROR RECUERDA DESACTIVAR EL HILO DEL NFC
	status_q=osMessageQueueGet(q_nfc,&uid,0,20);
			
	if(status_q==osOK){
		msg_tx.cmd= 0x70; //cmd de uid 
		msg_tx.length=uid.length-1;
		for(int i=0;i<uid.length-1;i++){
			msg_tx.buff[i]=uid.buff[i];
						
		}		
		osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
				
					
		}		
	if(flag_tim_rtc==1){
		msg_tx.cmd=0x20;
		msg_tx.length=0;
		osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
		flag_tim_rtc=0;
		
	}
}
void estado_Bajo_Consumo(void)
{
	
}

void comp_cmd(ComData_t com_data){
	switch(msg_rx.cmd){
		case RESP_HORA:
			
			sscanf(msg_rx.buff,"%d:%d:%d %d/%d/%d", ts.tm_hour,
																							ts.tm_min,
																							ts.tm_sec,
																							ts.tm_mday,
																							ts.tm_mon,
																							ts.tm_year);
			RTC_CalendarConfig(ts);
			break;
		case DORMIR: // AQUI HACE LA TRANSICION A MODO BAJO_CONSUMO
			
			//RTC_AlarmTypeDef alarma;
		sscanf((char*)msg_rx.buff,
           "%d:%d:%d %d/%d/%d-%d:%d:%d %d/%d/%d",
           &ts.tm_hour,  &ts.tm_min,  &ts.tm_sec,
           &ts.tm_mday,  &ts.tm_mon,  &ts.tm_year,
		       &alarma.AlarmTime.Hours,
           &alarma.AlarmTime.Minutes,
           &alarma.AlarmTime.Seconds);
		
		ts.tm_mon  -= 1;
		ts.tm_year -= 1900;
		
		RTC_CalendarConfig(ts);

    // Configurar alarma con la hora de despertar
		RTC_Set_AlarmWakeup(alarma.AlarmTime);

    estado = BAJO_CONSUMO;
		
			break;
			
			
		}
	
}

void TimerRTC_Callback(void const *arg){
	flag_tim_rtc=1;
	
}
