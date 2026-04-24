#ifndef __SD_H
#define __SD_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define BUFSD_MAX 128
#define MSGQUEUE_OBJECTS_SD 1

#define ALARM 0x01
#define NFC 0x02



typedef struct 
{
	char BufSD[BUFSD_MAX];
	
} MSGQUEUE_OBJ_SD;

extern osMessageQueueId_t mid_Msg_SD;

#endif

