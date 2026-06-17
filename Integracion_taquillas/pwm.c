#include "pwm.h"

TIM_HandleTypeDef htim1;
TIM_OC_InitTypeDef sConfigOC;

static void Init_PWM(void);
static void PWM_SetFrequency(uint32_t freq_hz);
static void PWM_Stop(void);

const osThreadAttr_t Thread_PWM_attr = {
    .stack_size = 1024
};

osThreadId_t tid_Thread_PWM;

/*----------------------------------------------------------------------------
*      Message Queue PWM
*---------------------------------------------------------------------------*/
MSGQUEUE_OBJ_PWM msg_PWM_leido;
osMessageQueueId_t mid_Msg_PWM;

/*----------------------------------------------------------------------------
*      Thread_PWM
*---------------------------------------------------------------------------*/
void Thread_PWM(void *argument);

int Init_Thread_PWM(void)
{
    mid_Msg_PWM = osMessageQueueNew(MSGQUEUE_OBJECTS_PWM,
                                    sizeof(MSGQUEUE_OBJ_PWM),
                                    NULL);
    if (mid_Msg_PWM == NULL)
    {
        return -1;
    }

    tid_Thread_PWM = osThreadNew(Thread_PWM, NULL, &Thread_PWM_attr);
    if (tid_Thread_PWM == NULL)
    {
        return -1;
    }

    return 0;
}

void Thread_PWM(void *argument)
{
    osStatus_t status;

    Init_PWM();
    PWM_Stop();

    while (1)
    {
        status = osMessageQueueGet(mid_Msg_PWM, &msg_PWM_leido, NULL, osWaitForever);

        if (status == osOK)
        {
            if (msg_PWM_leido.frecuencia == 0U)
            {
                PWM_Stop();
            }
            else
            {
                PWM_SetFrequency(msg_PWM_leido.frecuencia);
            }
        }
    }
}

static void Init_PWM(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = PWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 179;                       // 180 MHz / (179+1) = 1 MHz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 999;                          // Inicial: 1 kHz
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        while (1);
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;                            // 50 %
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        while (1);
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;

    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        while (1);
    }

    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        while (1);
    }

    __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_1);
}

static void PWM_SetFrequency(uint32_t freq_hz)
{
    uint32_t arr;
    uint32_t ccr;

    if (freq_hz < PWM_FREQ_MIN_HZ)
    {
        PWM_Stop();
        return;
    }

    if (freq_hz > PWM_FREQ_MAX_HZ)
    {
        freq_hz = PWM_FREQ_MAX_HZ;
    }

    arr = (1000000UL / freq_hz) - 1UL;

    if (arr < 1U)
    {
        arr = 1U;
    }

    ccr = (arr + 1UL) / 2UL;   // 50 %

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
    __HAL_TIM_SET_COUNTER(&htim1, 0U);

    __HAL_TIM_GENERATE_EVENT(&htim1, TIM_EVENTSOURCE_UPDATE);
}

static void PWM_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COUNTER(&htim1, 0U);
    __HAL_TIM_GENERATE_EVENT(&htim1, TIM_EVENTSOURCE_UPDATE);
}