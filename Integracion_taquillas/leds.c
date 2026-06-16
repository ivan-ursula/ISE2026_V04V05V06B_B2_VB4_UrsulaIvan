#include "leds.h"

extern uint8_t state;
static void Init_LD1(void);
static uint8_t flag;
static uint8_t cnt1 = 0; 
static uint8_t cnt3 = 0; 
uint32_t exec1;
/*----- Periodic Timer LEDs -----*/
osTimerId_t tim_id_led1;
osTimerId_t tim_id_led3;

// Periodic Timer Function
static void Timer_Callback_LED1 (void const *arg) {
  cnt1++;
  HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
  if(cnt1 >= 25){
    HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
    cnt1 = 0;
    osTimerStop(tim_id_led1);
  }
}
static void Timer_Callback_LED3 (void const *arg) {
  cnt3++;
  HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
  if(cnt3 >= 20){
    HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
    cnt3 = 0;
    osTimerStop(tim_id_led3);
  }
}

int Init_Timers (void) {

  exec1 = 1U;
  tim_id_led1 = osTimerNew((osTimerFunc_t)&Timer_Callback_LED1, osTimerPeriodic, &exec1, NULL);
//  if (tim_id_led1 != NULL) {
//    return -1; 
//  }
  tim_id_led3 = osTimerNew((osTimerFunc_t)&Timer_Callback_LED3, osTimerPeriodic, NULL, NULL);
  if (tim_id_led3 != NULL) {
    return -1; 
  }
    return NULL;
}
/*----------------------------------------------------------------------------
*      Thread LD1
 *---------------------------------------------------------------------------*/
const osThreadAttr_t Thread_LD1_attr = {.stack_size = 1024};
osThreadId_t tid_Thread_LD1;

void Thread_LD1 (void *argument);


int Init_Thread_LD1 (void) {
  Init_Timers();
  tid_Thread_LD1 = osThreadNew(Thread_LD1, NULL, &Thread_LD1_attr);
  if (tid_Thread_LD1 == NULL) {
    return(-1);
  }
  return(0);
}


void Thread_LD1 (void *argument) {
	Init_LD1();
	while (1) {
		flag = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
    HAL_GPIO_WritePin(GPIOB, LD1_Pin, (flag & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LD2_Pin, (flag & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LD3_Pin, (flag & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    if(flag & 0x10){
      HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
      osTimerStart(tim_id_led1, 200U);
    }
    if(flag & 0x20){
      HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
      osTimerStart(tim_id_led3, 200U);
    }
    osThreadYield();
  }
}

static void Init_LD1(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  GPIO_InitStruct.Pin = LD1_Pin |LD2_Pin |LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
} 
