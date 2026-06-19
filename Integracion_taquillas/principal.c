#include "cmsis_os2.h"     
#include "stm32f4xx_hal.h"
#include "prueba.h"
#include "RTC.h"
#include "pwm.h"
#include "vcnl.h"
#include "leds.h"
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
volatile uint8_t flag_alarma_wakeup = 0;

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

//VARIABLES HILO PRINCIPAL
estados_t estado;
RTC_AlarmTypeDef alarma;
uint32_t exec2;
uint16_t estado_led = 0;
void th_main (void *argument);                   
 
int Init_main (void) {
  
  thmain = osThreadNew(th_main, NULL, NULL);
  exec2=0;
	

  if (thmain == NULL) {
    return(-1);
  }
 
  return(0);
}

void th_main (void *argument) {
  estado= ACTIVO;
  
  
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
  
  status_q=osMessageQueueGet(q_nfc,&uid,0,20);
  if(status_q==osOK){
    msg_tx.cmd= LECTURA_NFC; //cmd de uid 
    msg_tx.length=uid.length-1;
    for(int i=0;i<uid.length-1;i++){
      msg_tx.buff[i]=uid.buff[i];
            
    }
    osDelay(50);
    osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
  }
  

  
}
void estado_Bajo_Consumo(void)
{
  osDelay(1000);
  msg_tx.length = 0;
  msg_tx.cmd = RESP_DORMIR;
  osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
  osDelay(1000);


  
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
  Init_PWM();
  Init_LDS();
  osThreadFlagsSet(th_id_VCNL, 0x04);
  
  if (flag_alarma_wakeup) {
    msg_tx.length = 0;
    msg_tx.cmd = RESP_DESPERTAR;
    osMessageQueuePut(qCom_Tx, &msg_tx, 0, 0);
    estado = ACTIVO;
    }
  
}
void estado_Alarma(void) 
{
  
  MSGQUEUE_OBJ_PWM msg;
  
  msg.frecuencia = 1;
  osMessageQueuePut(mid_Msg_PWM, &msg, 0U, 0U);
  
  osDelay(1000);
  msg_tx.cmd = ALARM; 
  msg_tx.length = 0;
  osMessageQueuePut(qCom_Tx, &msg_tx, 0, 0);
  osMessageQueueGet(qCom_Rx, &msg_rx, 0, osWaitForever);
  osDelay(1000);
  
  
  msg.frecuencia = 0;
  osMessageQueuePut(mid_Msg_PWM, &msg, 0U, 0U);
  
  if(msg_rx.cmd == RESP_ALARMA && msg_rx.buff[0] == 1){
    msg_tx.length = 0;
    msg_tx.cmd = RESP_DESPERTAR;
    osMessageQueuePut(qCom_Tx, &msg_tx, 0, 0);
    estado = ACTIVO;
  }
  else if(msg_rx.cmd == RESP_ALARMA && msg_rx.buff[0] == 2){
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
      RTC_CalendarShow(showtime, showdate); 

    break;
    
    case DORMIR:
      
    sscanf((char*)msg_rx.buff,
           "%d:%d",
           &ts_sleep.tm_hour,&ts_sleep.tm_min);
    ts_sleep.tm_sec = 0;
    
    RTC_Set_AlarmSleep(ts_sleep);        // Alarma B: cuándo dormirse

    break;
    
    case DESPERTAR:
    
    sscanf((char*)msg_rx.buff,
           "%d:%d",
           &ts_wake.tm_hour, &ts_wake.tm_min);
    ts_wake.tm_sec = 0;
    
    RTC_Set_AlarmWakeup(ts_wake);  // Alarma A: cuándo despertar
      
    break;
    case RESP_ESTADO_TAQUILLA:
        osThreadFlagsSet(tid_Thread_LD1, msg_rx.buff[0]);

    
    break;
    
    }
    if(msg_rx.cmd==LECTURA_CORRIENTE||msg_rx.cmd==LECTURA_PESO||msg_rx.cmd==LECTURA_TENSION){
      osMessageQueuePut(q_adc_peticion,&msg_rx.cmd,0,5);
      
      status_q=osMessageQueueGet(q_adc_data,&data_adc,0,100);
      elementos=osMessageQueueGetCount(q_adc_data);
      if(status_q==osOK){
        if(data_adc.cmd==RESP_LECTURA_PESO){
          msg_tx.cmd = RESP_LECTURA_PESO;
          msg_tx.length=sprintf((char*)msg_tx.buff ,"%.2f-%.2f",data_adc.valor1,data_adc.valor2);
          osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
//          osDelay(2000);
          msg_tx.cmd= HORA;
          msg_tx.length=0;
          osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
          
        }else if(data_adc.cmd==RESP_LECTURA_TENSION){
          
          msg_tx.length=sprintf((char*)msg_tx.buff,"%.2f",data_adc.valor1);
          msg_tx.cmd = RESP_LECTURA_TENSION;
          osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
          
        } else if(data_adc.cmd==RESP_LECTURA_CORRIENTE) {
          
          msg_tx.length=sprintf((char*)msg_tx.buff,"%.2f",data_adc.valor1);
          msg_tx.cmd = RESP_LECTURA_CORRIENTE;
          osMessageQueuePut(qCom_Tx,&msg_tx,0,0);
        }
      }		
      
    }
}
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(INT_PIN);
}
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
  if (pin == INT_PIN )
  {
    
    osThreadFlagsSet(th_id_VCNL, 0x02);
    if( estado == BAJO_CONSUMO) 
    {
    estado = ALARMA;
    flag_alarma_wakeup = 0;
    }


  }
}
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
  
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    estado = ACTIVO;
    flag_alarma_wakeup = 1;
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  estado = BAJO_CONSUMO;
}
