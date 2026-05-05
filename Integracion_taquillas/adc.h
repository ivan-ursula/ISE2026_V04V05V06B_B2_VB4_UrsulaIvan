#include "stm32f4xx_hal.h"
#ifndef __ADC_H
#define RESOLUTION_12B 4096U
#define VREF 3.3f
#define delta_taquilla 3.3f/4096
#define CANAL_T1 10
#define CANAL_T2 11

typedef struct{
	
	float valor1;
	float valor2;
	uint8_t cmd;
} msg_adc_data;

void ADC1_pins_F429ZI_config(void);
int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
uint32_t ADC_getData(ADC_HandleTypeDef * , uint32_t );
#endif
