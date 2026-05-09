#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "adc.h"
#include "com.h"
 
osThreadId_t th_ADC;                        
osMessageQueueId_t q_adc_peticion;
osMessageQueueId_t q_adc_data;
ADC_HandleTypeDef adc;
void thread_ADC (void *argument);                  
 
int Init_thADC(void) {
 
  th_ADC = osThreadNew(thread_ADC, NULL, NULL);
	q_adc_peticion=osMessageQueueNew(5,sizeof(uint8_t),NULL);
	
	q_adc_data=osMessageQueueNew(5,sizeof(msg_adc_data),NULL);
  if (th_ADC == NULL) {
    return(-1);
  }
 
  return(0);
}
uint8_t peticion;
msg_adc_data dato;
void thread_ADC (void *argument) {
	ADC1_pins_F429ZI_config();
	ADC_Init(&adc, ADC1);
  while (1) {
		osMessageQueueGet(q_adc_peticion,&peticion,0,osWaitForever);
		
		if(peticion==LECTURA_PESO){
			dato.valor1=ADC_getData(&adc,CANAL_T1)*delta_taquilla;
			dato.valor2=ADC_getData(&adc,CANAL_T2)*delta_taquilla;
			
			dato.cmd=RESP_LECTURA_PESO;
			
			
		}
	  if(peticion==LECTURA_TENSION){
			dato.valor1=ADC_getData(&adc,CANAL_TENSION)*delta_taquilla*2;
			//dato.valor2=ADC_getData(&adc,CANAL_T2)*delta_taquilla;
			
			dato.cmd=RESP_LECTURA_TENSION;
			
			
		}
		if(peticion==LECTURA_CORRIENTE){
			dato.valor1=ADC_getData(&adc,CANAL_CORRIENTE)*delta_taquilla;
			//dato.valor2=ADC_getData(&adc,CANAL_T2)*delta_taquilla;
			
			dato.cmd=RESP_LECTURA_CORRIENTE;
			
			
		}
		
		osMessageQueuePut(q_adc_data,&dato,0,0);
  }
}
