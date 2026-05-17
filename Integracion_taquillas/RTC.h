#ifndef __RTC_H
#define __RTC_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include <time.h>
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

void Init_RTC(void);
void RTC_CalendarConfig(struct tm ts);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
void time_callback(uint32_t seconds, uint32_t seconds_fraction);

void RTC_Set_AlarmWakeup(struct tm ts_wake);
void RTC_Set_AlarmSleep(struct tm ts_sleep);

typedef struct{
  uint8_t hora[50];
  uint8_t fecha[50];
} rtc_t;

extern RTC_HandleTypeDef RtcHandle;
#endif
