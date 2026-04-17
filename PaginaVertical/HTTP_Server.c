/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

#include "LCD.h"
#include "rtc.h"
#include "adc.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

/* Timer IDs */
osTimerId_t tim_id1;
osTimerId_t tim_idBlink;
static uint32_t exec1;
static uint32_t exec2;
static int init_Timer (void);

/* Thread IDs */
osThreadId_t thLed;
static uint8_t flag = 0x00;
const osThreadAttr_t Thread_LED_attr = {.stack_size = 1024};

/* Functions */
static void blink (void *arg);

__NO_RETURN void app_main (void *arg);

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
  }
}

 /*----------------------------------------------------------------------------
*      Interrupcion boton azul
 *---------------------------------------------------------------------------*/
void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//  struct tm ts;
//  ts.tm_year=2000-100-8;
//  ts.tm_mday=1;
//  ts.tm_mon=1-1;
//  ts.tm_hour=0;
//  ts.tm_min=0;
//  ts.tm_sec=0;
//  RTC_CalendarConfig(ts);
  netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback);
}

/*----------------------------------------------------------------------------
  Inicializar LED
 *---------------------------------------------------------------------------*/
static void initi_gpio_Led(void){
  GPIO_InitTypeDef timgpio;
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  timgpio.Pin = GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14;
  timgpio.Mode = GPIO_MODE_OUTPUT_PP;
  timgpio.Pull = GPIO_NOPULL;
  timgpio.Speed = GPIO_SPEED_FREQ_LOW;
  
  HAL_GPIO_Init(GPIOB, &timgpio);
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*----------------------------------------------------------------------------
  Thread LED INIT
 *---------------------------------------------------------------------------*/
int init_thLed (void){
  thLed = osThreadNew(blink, NULL, &Thread_LED_attr);
  if (thLed == NULL){
    return(-1);
  }
  return(0);
}

static void blink (void *arg){
  initi_gpio_Led();
  
  while(1){
    flag = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
    
    if(flag & 0x08){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
    
    if(flag & 0x01){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }else if((flag & 0x01) == 0){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }
    
    if(flag & 0x02){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
      
    }else if((flag & 0x02) == 0){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    }
    
    if(flag & 0x04){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }else if((flag & 0x04) == 0){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
    
//    if(flag & 0x10){
//      osTimerStart(tim_id1, 200U); 
//    }
    
    if(flag & 0x20){
      osTimerStart(tim_idBlink, 200U); 
    }
    
    osThreadYield();
  }
}

/*----------------------------------------------------------------------------
*      Callback alarma
 *---------------------------------------------------------------------------*/
static uint8_t counter = 1;
static uint8_t counter2 = 1;

static void Timer1_Callback (void const *arg) {
  if(counter < 25){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    counter++;
  }else{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    counter = 1;
    osTimerStop(tim_id1);
  }
}

static void Timer2_Callback (void const *arg) {
  if(counter2 < 20){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
    counter2++;
  }else{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    counter2 = 1;
    osTimerStop(tim_idBlink);
  }
}

//Create and Start timers
int init_Timer (void) {
  exec1 = 1U;
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerPeriodic, &exec1, NULL);
  
  exec2 = 1U;
  tim_idBlink = osTimerNew((osTimerFunc_t)&Timer2_Callback, osTimerPeriodic, &exec2, NULL);
  
  return NULL;
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;
  
  init_thLed();
  init_Thread_LCD ();
  init_thADC ();
  init_Timer();
  netInitialize ();
  osDelay(5000);
  init_Thread_RTC();

  osThreadExit();
}
