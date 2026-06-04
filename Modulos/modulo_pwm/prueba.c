#include "pwm.h"

const osThreadAttr_t Thread_prueba_attr = {
    .stack_size = 1024
};

osThreadId_t tid_Thread_prueba;
void Thread_prueba(void *argument);

MSGQUEUE_OBJ_PWM prueba;

/*----------------------------------------------------------------------------
*      Thread_Prueba
*---------------------------------------------------------------------------*/

int Init_Thread_prueba(void)
{
    tid_Thread_prueba = osThreadNew(Thread_prueba, NULL, &Thread_prueba_attr);
    if (tid_Thread_prueba == NULL) {
        return -1;
    }

    return 0;
}

void Thread_prueba(void *argument)
{
	const uint32_t frecuencias[] = {200, 400, 500, 1000, 2000, 3000, 4000, 0};
	uint32_t i = 0;

	while (1)
	{
		if (mid_Msg_PWM != NULL)
		{
				prueba.frecuencia = frecuencias[i];
				osMessageQueuePut(mid_Msg_PWM, &prueba, 0U, 0U);

				i++;
				if (i >= (sizeof(frecuencias) / sizeof(frecuencias[0])))
				{
						i = 0;
				}
		}

	osDelay(1000);   // 1 segundo
	}
}