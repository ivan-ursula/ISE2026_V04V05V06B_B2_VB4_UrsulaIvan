#include "cmsis_os2.h"     
#include "stm32f4xx_hal.h"
#include "prueba.h"
#include "vcnl.h"

#include <time.h>

//ID DE HILOS
osThreadId_t thmain;  
//ID DE COLAS DE MENSAJES
extern osMessageQueueId_t q_nfc;
extern osMessageQueueId_t qCom_Tx;
extern osMessageQueueId_t qCom_Rx;
extern osMessageQueueId_t q_adc_peticion;
extern osMessageQueueId_t q_adc_data;

//VARIABLES DE COLAS DE MENSAJES
msg_nfc uid;
ComData_t msg_tx;
ComData_t msg_rx;
msg_adc_data data_adc;
uint8_t status_q;

//TIMER HILO PRINCIPAL
osTimerId_t tim_RTC;

//FUNCIONES DE LOS ESTADOS
void estado_Activo(void);
void estado_Bajo_Consumo(void);
void estado_Alarma(void);

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
  estado= ACTIVO;
	while (1) {
		
		

		
		switch(estado){
			case ACTIVO:
				
			estado_Activo();

				
				break ;
			case BAJO_CONSUMO:
				
			estado_Bajo_Consumo();
				
				break;
			case ALARMA:
				
			estado_Alarma();
			
				break;			
			
		}
		
		
    
                       
  }
}
uint8_t elementos;

void estado_Activo(void)
{
	status_q=osMessageQueueGet(qCom_Rx,&msg_rx,0,20);
	if(status_q==osOK){
		comp_cmd(msg_rx);
	}			

	//SI QUIERES PROBAR SIN NFC PUEDE QUE TE DE EROR RECUERDA DESACTIVAR EL HILO DEL NFC
	status_q=osMessageQueueGet(q_nfc,&uid,0,20);
	if(status_q==osOK){
		msg_tx.cmd= LECTURA_NFC; //cmd de uid 
		msg_tx.length=uid.length-1;
		for(int i=0;i<uid.length-1;i++){
			msg_tx.buff[i]=uid.buff[i];
						
		}		
		osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
	}
	
	if(flag_tim_rtc==1){
		msg_tx.cmd= HORA;
		msg_tx.length=0;
		osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
		flag_tim_rtc=0;
	}
	
}

void estado_Bajo_Consumo(void)
{ 
	/* Tengo que meterme en el modo bajo consumo. Tengo que poner algo en el handler de la
	interrupcion? Tengo que reactivar el resto de pines*/
	StopMode_Measure();
	estado = ACTIVO;
	
}
void estado_Alarma(void) 
{
	msg_tx.cmd = ALARM; 
	msg_tx.length = 0x04;
	//msg_tx.buff = ; 
	osMessageQueuePut(qCom_Tx, &msg_tx, 0, 0);
	osMessageQueueGet(qCom_Rx, &msg_rx, 0, osWaitForever);
	if(msg_rx.cmd == RESP_ALARMA){
		estado = ACTIVO;
	}
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
//		sscanf((char*)msg_rx.buff,
//           "%d:%d:%d %d/%d/%d-%d:%d:%d %d/%d/%d",
//           &ts.tm_hour,  &ts.tm_min,  &ts.tm_sec,
//           &ts.tm_mday,  &ts.tm_mon,  &ts.tm_year,
//		       &alarma.AlarmTime.Hours,
//           &alarma.AlarmTime.Minutes,
//           &alarma.AlarmTime.Seconds);
//		
//		ts.tm_mon  -= 1;
//		ts.tm_year -= 1900;
//		
//		RTC_CalendarConfig(ts);

    // Configurar alarma con la hora de despertar
		//RTC_Set_AlarmWakeup(alarma.AlarmTime);
		osDelay(5000);
    estado = BAJO_CONSUMO;
		
			break;
			
			
		}
		if(msg_rx.cmd==LECTURA_CORRIENTE||msg_rx.cmd==LECTURA_PESO||msg_rx.cmd==LECTURA_TENSION){
			osMessageQueuePut(q_adc_peticion,&msg_rx.cmd,0,5);
			
			status_q=osMessageQueueGet(q_adc_data,&data_adc  ,0,100);
			elementos=osMessageQueueGetCount(q_adc_data);
			if(status_q==osOK){
				if(data_adc.cmd==RESP_LECTURA_PESO){
					msg_tx.length=sprintf(msg_tx.buff,"%.2f-%.2f",data_adc.valor1,data_adc.valor2);
					osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
					
				}else{
					msg_tx.length=sprintf(msg_tx.buff,"%.2f",data_adc.valor1);
					osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
				}
			}		
			
		}
}

void TimerRTC_Callback(void const *arg){
	flag_tim_rtc=1;
	
}
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == INT_PIN)
    {
			//osThreadFlagsSet(th_id_VCNL, 0x02);
			estado = ALARMA;
    }
		if (pin == GPIO_PIN_13)
    {
        // El botón despertó al micro del modo STOP
        // Aquí puedes poner una bandera, etc.
    }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  //osThreadFlagsSet(tid_Thread_LD1, 0x10);
	estado = ACTIVO;
}
