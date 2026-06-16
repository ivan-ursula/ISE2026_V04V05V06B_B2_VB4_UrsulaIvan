#ifndef __PWM_H
#define __PWM_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

/* TIM1_CH1 en PE9 */
#define PWM_PIN               GPIO_PIN_9
#define MSGQUEUE_OBJECTS_PWM  1

/* Rango útil para buzzer pasivo */
#define PWM_FREQ_MIN_HZ       20U
#define PWM_FREQ_MAX_HZ       20000U

typedef struct {
    uint32_t frecuencia;   // 0 = buzzer apagado
} MSGQUEUE_OBJ_PWM;

extern osMessageQueueId_t mid_Msg_PWM;

int Init_Thread_PWM(void);
void Thread_PWM(void *argument);

#endif