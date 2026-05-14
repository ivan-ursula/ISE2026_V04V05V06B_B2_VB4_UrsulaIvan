#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "string.h"
#include <time.h>

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

int init_Thread_RTC (void);

void RTC_CalendarConfig(struct tm ts);

/*Msg queue for date-hour*/
typedef struct {
  char BufHour[50];
  char BufDate[50];
} MSGQUEUE_OBJ_DATE;
extern osMessageQueueId_t mid_Msg_Date;

extern MSGQUEUE_OBJ_DATE fecha_rec;

#endif
