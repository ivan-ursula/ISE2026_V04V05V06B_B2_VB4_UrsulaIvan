#include "RTC.h"
#include <stdio.h>
#include <string.h>


RTC_HandleTypeDef RtcHandle;
extern osThreadId_t thLed;

//const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 123, 216, 239, 35, 4 };
struct tm ts;
struct tm ts_alarma;
void Error_Handler(void)
{
  /* Manda mensaje  */
  printf("Hubo un error al inicializar el RTC");
  while (1)
  {
  }
}

void RTC_CalendarConfig(struct tm ts)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Lunes 2 de Marzo de 2026 */
  sdatestructure.Year = ts.tm_year-100;
  sdatestructure.Month = ts.tm_mon+1;
  sdatestructure.Date = ts.tm_mday;
  sdatestructure.WeekDay = RTC_WEEKDAY_MONDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 14:00:00 */
  stimestructure.Hours = ts.tm_hour;
  stimestructure.Minutes = ts.tm_min;
  stimestructure.Seconds = ts.tm_sec;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  
  
  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
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
  
  /* Inicializar la alarma*/
  
//  RTC_AlarmTypeDef alarma;
//  alarma.AlarmTime.Hours=0;
//  alarma.AlarmTime.Minutes=0;
//  alarma.AlarmTime.Seconds=5;
//  alarma.AlarmTime.SubSeconds=0;
//	
//	
//  alarma.AlarmTime.TimeFormat=RTC_HOURFORMAT_24;
//  
//  alarma.AlarmTime.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
//  alarma.AlarmTime.StoreOperation=RTC_STOREOPERATION_RESET;
//  
//  alarma.AlarmMask=RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
//  
//  alarma.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_ALL;
//  alarma.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_DATE;
//  
//  alarma.AlarmDateWeekDay=0x01;
//  alarma.Alarm=RTC_ALARM_A;
//  HAL_RTC_SetAlarm_IT(&RtcHandle,&alarma,RTC_FORMAT_BCD);
//  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
//  
//  /* Inicializar la WAKEUP*/
//  
//  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
//  HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 8191 , RTC_WAKEUPCLOCK_RTCCLK_DIV16);//32k/16=2k  p=0.5ms
  
  //RTC_CalendarConfig();
  
  
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
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : dd-mm-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
}
void RTC_Set_AlarmWakeup(struct tm ts_wake)
{
    RTC_AlarmTypeDef alarma;

    /* --- Tiempo de la alarma --- */
    alarma.AlarmTime.Hours          = ts_wake.tm_hour;
    alarma.AlarmTime.Minutes        = ts_wake.tm_min;
    alarma.AlarmTime.Seconds        = ts_wake.tm_sec;

    alarma.AlarmTime.TimeFormat     = RTC_HOURFORMAT_24;
    alarma.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarma.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    /* --- Máscara: comparar TODO (día + hh:mm:ss) ---
       En el código comentado se enmascaraban día/horas/minutos ? solo comparaba segundos.
       Aquí queremos despertar en una fecha+hora exacta, así que AlarmMask = NONE */
    alarma.AlarmMask                = RTC_ALARMMASK_NONE;

    alarma.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;

    /* --- Fecha del día de despertar --- */
    alarma.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarma.AlarmDateWeekDay         = ts_wake.tm_mday;

    alarma.Alarm                    = RTC_ALARM_A;

//    /* --- Desactivar por si había una alarma previa --- */
//    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);

    /* --- Activar con interrupción (BIN, igual que SetDate/SetTime) --- */
    if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarma, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
		
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}


//HANDler no tocar
void RTC_Alarm_IRQHandler(void){
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
  
}

void RTC_WKUP_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}

//void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
//{
//  //osThreadFlagsSet(tid_Thread_LD1, 0x10);
//}

void time_callback(uint32_t seconds, uint32_t seconds_fraction){
 
 struct tm* ptr_ts;
  time_t sys_time=(time_t)seconds +3600;
  ptr_ts=localtime(&sys_time);
  ts=*ptr_ts;
  RTC_CalendarConfig(ts);
  //ts=*localtime(&sys_time);


}

