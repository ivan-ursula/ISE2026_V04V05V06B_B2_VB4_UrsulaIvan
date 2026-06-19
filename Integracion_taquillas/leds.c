#include "leds.h"

extern uint8_t state;
static uint8_t flag;
uint32_t exec1;

/*----------------------------------------------------------------------------
*      Thread LD1
 *---------------------------------------------------------------------------*/
const osThreadAttr_t Thread_LD1_attr = {.stack_size = 1024};
osThreadId_t tid_Thread_LD1;

void Thread_LD1 (void *argument);


int Init_Thread_LD1 (void) {
  tid_Thread_LD1 = osThreadNew(Thread_LD1, NULL, &Thread_LD1_attr);
  if (tid_Thread_LD1 == NULL) {
    return(-1);
  }
  return(0);
}


void Thread_LD1 (void *argument) {
	Init_LDS();
	while (1) {
		flag = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
    if ((flag & 0x01)){
      HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
    }
    if ((flag & 0x02)){
      HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
    }

    osThreadYield();
  }
}

void Init_LDS(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  GPIO_InitStruct.Pin = LD1_Pin | LD2_Pin |LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
} 
