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
extern osMessageQueueId_t q_adc_peticion;
extern osMessageQueueId_t q_adc_data;
extern ADC_HandleTypeDef adc;

uint8_t showtime[10];
uint8_t showdate[12];
uint8_t elementos;

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
  exec2=0;
	tim_RTC=osTimerNew((osTimerFunc_t)&TimerRTC_Callback, osTimerPeriodic, &exec2, NULL);
	

  if (thmain == NULL) {
    return(-1);
  }
 
  return(0);
}

void th_main (void *argument) {
  estado= ACTIVO;
	osTimerStart(tim_RTC,1000);
	Init_RTC();
	
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
		
		RTC_CalendarShow(showtime, showdate);
	}
	
}

void estado_Bajo_Consumo(void)
{ 
	osTimerStop(tim_RTC);
  StopMode_Measure();
  
  osKernelResume(0);
  desinit_uart();
  init_uart();
	osThreadFlagsSet(thcom_Rx, 0x1);
  NFC_Deinit_SPI();
  NFC_init_SPI();
  ADC1_pins_F429ZI_config();
  Deinit_ADC(&adc);
  ADC_Init(&adc, ADC1);
  
  osTimerStart(tim_RTC, 1000);
	
}
void estado_Alarma(void) 
{
	msg_tx.cmd = ALARM; 
	msg_tx.length = 0x00;
	osMessageQueuePut(qCom_Tx, &msg_tx, 0, 0);
	osMessageQueueGet(qCom_Rx, &msg_rx, 0, osWaitForever);
	
	if(msg_rx.cmd == RESP_ALARMA && msg_tx.buff[1] == 1){
		estado = ACTIVO;
	}
	else if(msg_rx.cmd == RESP_ALARMA && msg_tx.buff[1] == 0){
		estado = BAJO_CONSUMO; 
	}
}
void comp_cmd(ComData_t com_data){
	
	struct tm ts_wake;
	struct tm ts_sleep;
	struct tm ts;
	
	switch(msg_rx.cmd){
		
		case RESP_HORA:
			sscanf((char*)msg_rx.buff, "%d:%d:%d %d/%d/%d",
			&ts.tm_hour, &ts.tm_min,  &ts.tm_sec,
			&ts.tm_mday, &ts.tm_mon,  &ts.tm_year);
			RTC_CalendarConfig(ts);

    break;
		case DORMIR:
			
		
		sscanf((char*)msg_rx.buff,
           "%d:%d:%d %d/%d/%d-%d:%d:%d %d/%d/%d",
           &ts_sleep.tm_hour,&ts_sleep.tm_min,&ts_sleep.tm_sec,
           &ts_sleep.tm_mday,&ts_sleep.tm_mon,&ts_sleep.tm_year,
           &ts_wake.tm_hour, &ts_wake.tm_min, &ts_wake.tm_sec,
           &ts_wake.tm_mday, &ts_wake.tm_mon, &ts_wake.tm_year);
		
    RTC_Set_AlarmSleep(ts_sleep);        // Alarma B: cuándo dormirse
    RTC_Set_AlarmWakeup(ts_wake);  // Alarma A: cuándo despertar

    break;
			
		}
		if(msg_rx.cmd==LECTURA_CORRIENTE||msg_rx.cmd==LECTURA_PESO||msg_rx.cmd==LECTURA_TENSION){
			osMessageQueuePut(q_adc_peticion,&msg_rx.cmd,0,5);
			
			status_q=osMessageQueueGet(q_adc_data,&data_adc  ,0,100);
			elementos=osMessageQueueGetCount(q_adc_data);
			if(status_q==osOK){
				if(data_adc.cmd==RESP_LECTURA_PESO){
					msg_tx.length=sprintf((char*)msg_tx.buff ,"%.2f-%.2f",data_adc.valor1,data_adc.valor2);
					osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
					
				}else{
					msg_tx.length=sprintf((char*)msg_tx.buff,"%.2f",data_adc.valor1);
					osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
				}
			}		
			
		}
}

void TimerRTC_Callback(void const *arg){
	flag_tim_rtc=1;
	
}
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(INT_PIN);
}
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == INT_PIN)
    {
		  osThreadFlagsSet(th_id_VCNL, 0x02);
			estado = ALARMA;
    }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
    estado = ACTIVO;        // Alarma A ? despertar
}

void HAL_RTC_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
    estado = BAJO_CONSUMO;  // Alarma B ? dormir
}
