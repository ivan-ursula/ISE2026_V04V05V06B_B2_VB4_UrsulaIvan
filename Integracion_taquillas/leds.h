#ifndef __LED1_H
#define __LED1_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define LD1_Pin GPIO_PIN_0
#define LD2_Pin GPIO_PIN_7
#define LD3_Pin GPIO_PIN_14

/* Thread LED1 */
int Init_Thread_LD1 (void);

extern osThreadId_t tid_Thread_LD1;

#endif
