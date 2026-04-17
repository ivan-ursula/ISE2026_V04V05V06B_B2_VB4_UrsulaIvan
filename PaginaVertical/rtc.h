#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "string.h"
#include <time.h>
#include "rl_net.h"

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

int init_Thread_RTC (void);

//void RTC_Alarm_Period(void);
void RTC_CalendarConfig(struct tm ts);

/*Msg queue for date-hour*/
typedef struct {
  char BufHour[50];
  char BufDate[50];
} MSGQUEUE_OBJ_DATE;
extern osMessageQueueId_t mid_Msg_Date;
extern const NET_ADDR4 ntp_server;
extern void time_callback (uint32_t seconds, uint32_t seconds_fraction);

#endif
