#include "stm32f4xx_hal.h"
#ifndef __ADC_H
#define RESOLUTION_12B 4096U

#define delta 3.3f/4096
#define sensibilidad_taquilla 0.00066f// [v]/[g]

#define sensibilidad_corriente 4.199f // v/a

#define CANAL_T1 0
#define CANAL_T2 4
#define CANAL_CORRIENTE 13
#define CANAL_TENSION 9


typedef struct{
	
	float valor1;
	float valor2;
	uint8_t cmd;
} msg_adc_data;

void ADC1_pins_F429ZI_config(void);
int ADC_Init(ADC_HandleTypeDef *, ADC_TypeDef  *);
void Deinit_ADC(ADC_HandleTypeDef *);
uint32_t ADC_getData(ADC_HandleTypeDef * , uint32_t );
#endif
