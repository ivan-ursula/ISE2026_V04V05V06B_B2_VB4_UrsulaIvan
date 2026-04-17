#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#ifndef __ADC_H
  int init_thADC (void);
  void ADC1_pins_F429ZI_config(void);
  int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
  float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
  
#endif
