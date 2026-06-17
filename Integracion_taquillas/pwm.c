#include "pwm.h"

TIM_HandleTypeDef htim1;



const osThreadAttr_t Thread_PWM_attr = {
    .stack_size = 1024
};

osThreadId_t tid_Thread_PWM;
MSGQUEUE_OBJ_PWM msg_PWM_leido;
osMessageQueueId_t mid_Msg_PWM;

void Thread_PWM(void *argument);

int Init_Thread_PWM(void)
{
    mid_Msg_PWM = osMessageQueueNew(MSGQUEUE_OBJECTS_PWM,
                                    sizeof(MSGQUEUE_OBJ_PWM),
                                    NULL);
    if (mid_Msg_PWM == NULL) return -1;

    tid_Thread_PWM = osThreadNew(Thread_PWM, NULL, &Thread_PWM_attr);
    if (tid_Thread_PWM == NULL) return -1;

    return 0;
}

void Thread_PWM(void *argument)
{
    osStatus_t status;
    Init_PWM();
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    while (1)
    {
        status = osMessageQueueGet(mid_Msg_PWM, &msg_PWM_leido, NULL, osWaitForever);

        if (status == osOK)
        {
            if (msg_PWM_leido.frecuencia == 0U)
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);   // apagar
            else
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  // encender
        }
    }
}

void Init_PWM(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin       = PWM_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 179;       // 180 MHz / 180 = 1 MHz
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = 1999;       // 1 MHz / 500 = 2 kHz
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim1);

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 250;              // 50% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
}