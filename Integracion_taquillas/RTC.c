#include "RTC.h"
#include <stdio.h>
#include <string.h>


RTC_HandleTypeDef RtcHandle;

void Error_Handler(void)
{
  /* Manda mensaje  */
  printf("Hubo un error al inicializar el RTC");
  while (1)
  {
  }
}

void RTC_CalendarConfig(struct tm ts_conf)
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    // Calcular día de la semana automáticamente
	  ts_conf.tm_mon  -= 1;
    ts_conf.tm_year -= 1900;
    
	  mktime(&ts_conf);  // rellena ts.tm_wday (0=domingo, 1=lunes...)

    // tm_wday: 0=domingo ? HAL: 7=domingo, 1=lunes...
    uint8_t weekday = (ts_conf.tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : ts_conf.tm_wday;

    sdatestructure.Year    = ts_conf.tm_year - 100;
    sdatestructure.Month   = ts_conf.tm_mon + 1;
    sdatestructure.Date    = ts_conf.tm_mday;
    sdatestructure.WeekDay = weekday;

    if(HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN) != HAL_OK)
        Error_Handler();

    stimestructure.Hours          = ts_conf.tm_hour;
    stimestructure.Minutes        = ts_conf.tm_min;
    stimestructure.Seconds        = ts_conf.tm_sec;
    stimestructure.TimeFormat     = RTC_HOURFORMAT_24;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
        Error_Handler();

    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

void Init_RTC(void)
 {
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct; 
  
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
   
   
   
    __HAL_RCC_RTC_ENABLE();
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	__HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle, RTC_FLAG_ALRBF);  // limpiar flag Alarma B
	__HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle, RTC_FLAG_ALRAF);  // limpiar flag Alarma A
  
}

void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : dd-mm-yy */
  sprintf((char *)showdate, "%02d-%02d-%02d", sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
}

void RTC_Set_AlarmWakeup(struct tm ts_wake)
{
    RTC_AlarmTypeDef alarma;

    alarma.AlarmTime.Hours          = ts_wake.tm_hour;
    alarma.AlarmTime.Minutes        = ts_wake.tm_min;
    alarma.AlarmTime.Seconds        = ts_wake.tm_sec;
    alarma.AlarmTime.TimeFormat     = RTC_HOURFORMAT_24;
    alarma.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarma.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    alarma.AlarmMask                = RTC_ALARMMASK_NONE;
    alarma.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;
    alarma.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarma.AlarmDateWeekDay         = ts_wake.tm_mday;
    alarma.Alarm                    = RTC_ALARM_A;

    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
    if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarma, RTC_FORMAT_BIN) != HAL_OK)
        Error_Handler();

    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void RTC_Set_AlarmSleep(struct tm ts_sleep)
{
    RTC_AlarmTypeDef alarma;

    alarma.AlarmTime.Hours          = ts_sleep.tm_hour;
    alarma.AlarmTime.Minutes        = ts_sleep.tm_min;
    alarma.AlarmTime.Seconds        = ts_sleep.tm_sec;
    alarma.AlarmTime.TimeFormat     = RTC_HOURFORMAT_24;
    alarma.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarma.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    alarma.AlarmMask                = RTC_ALARMMASK_NONE;
    alarma.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;
    alarma.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarma.AlarmDateWeekDay         = ts_sleep.tm_mday;
    alarma.Alarm                    = RTC_ALARM_B;
	
	  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
    if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarma, RTC_FORMAT_BIN) != HAL_OK)
        Error_Handler();

    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);  // mismo vector que Alarma A
}


