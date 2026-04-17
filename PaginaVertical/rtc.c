#include "rtc.h"

//extern thLed;
extern osThreadId_t thLed;

/* SNTP */
const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 123, 216, 239, 35, 4 };
void time_callback (uint32_t seconds, uint32_t seconds_fraction);
static struct tm ts;

/* Timer IDs */
osTimerId_t tim_idRTC;
static uint32_t exec1;
static int init_TimerRTC (void);

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

/* Private function prototypes -----------------------------------------------*/
static void RTC_CalendarShow(char *showtime, char *showdate);
static void Init_RTC(void);

/* Thread_RTC*/
osThreadId_t tid_ThRTC;
void Thread_RTC (void *argument);

/* Message */
MSGQUEUE_OBJ_DATE msg_DATE_leido;
MSGQUEUE_OBJ_DATE msg_env_web;
osMessageQueueId_t mid_Msg_Date;
osStatus_t status_Date_estado;
int Init_MsgQueue_Date (void);

/* Stack */
const osThreadAttr_t Thread_RTC_attr = {.stack_size = 1024};

/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
int Init_MsgQueue_Date (void) {

  mid_Msg_Date = osMessageQueueNew(1, sizeof(MSGQUEUE_OBJ_DATE), NULL);
  if (mid_Msg_Date == NULL) {
    return(-1);
  }
	
	return(0);
}

/*----------------------------------------------------------------------------
*      Inicializaciones
 *---------------------------------------------------------------------------*/
static void Init_RTC(void){
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc){
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    /* Initialization Error */
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    /* Initialization Error */
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
  
}


/*----------------------------------------------------------------------------
*      Funciones de RTC
 *---------------------------------------------------------------------------*/
void RTC_CalendarConfig(struct tm ts){
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
//  RTC_AlarmTypeDef sAlarm = {0};

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = ts.tm_year-100;
  sdatestructure.Month = ts.tm_mon+1;
  sdatestructure.Date = ts.tm_mday;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = ts.tm_hour;
  stimestructure.Minutes = ts.tm_min;
  stimestructure.Seconds = ts.tm_sec;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  
  /*##-3- Configure the Alarm #################################################*/
  /* Set Time: XX:XX:00 */
//  sAlarm.AlarmTime.Hours = 0x00;
//  sAlarm.AlarmTime.Minutes = 0x00;
//  sAlarm.AlarmTime.Seconds = 0x00;
//  sAlarm.AlarmMask = RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_DATEWEEKDAY;
//  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
//  sAlarm.Alarm = RTC_ALARM_A;
//  HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarm, RTC_FORMAT_BCD);
  
  /*Alarma periodica*/
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 40000, RTC_WAKEUPCLOCK_RTCCLK_DIV16); //32k/16 = 2k

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

static void RTC_CalendarShow(char *showtime, char *showdate){
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
}


/*----------------------------------------------------------------------------
*      IRQ alarma
 *---------------------------------------------------------------------------*/
//void RTC_Alarm_IRQHandler(void){
//  HAL_RTC_AlarmIRQHandler(&RtcHandle);
//}

//void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
//  struct tm *ptr_ts;
//  time_t sys_time=(time_t)1774259400;
//  ptr_ts=localtime(&sys_time);
//  ts=*ptr_ts;
//  RTC_CalendarConfig(ts);
//}
/*----------------------------------------------------------------------------
*      IRQ alarma periodica
 *---------------------------------------------------------------------------*/
  uint8_t count = 1;

void RTC_Alarm_Period(void){
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
//  HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 120000, RTC_WAKEUPCLOCK_RTCCLK_DIV16); //32k/16 = 2k -- (1/2k)*120000 = 60 segs
}

void RTC_WKUP_IRQHandler(void){
  HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
  if (count == 3){
  struct tm *ptr_ts;
  time_t sys_time=(time_t)1774259400;
  ptr_ts=localtime(&sys_time);
  ts=*ptr_ts;
  RTC_CalendarConfig(ts);
  count = 1;
  }else{
    count += 1;
  }
}
/*----------------------------------------------------------------------------
*      SNTP
 *---------------------------------------------------------------------------*/

void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  struct tm *ptr_ts;
  time_t sys_time=(time_t)seconds +3600;
  ptr_ts=localtime(&sys_time);
  ts=*ptr_ts;
  RTC_CalendarConfig(ts);
 }

 /*----------------------------------------------------------------------------
*      Timer RTC
 *---------------------------------------------------------------------------*/
static void TimerRTC_Callback (void const *arg) {
  netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback);
  osThreadFlagsSet(thLed, 0x20);
}

//Create and Start timers
int init_TimerRTC (void) {
  exec1 = 1U;
  tim_idRTC = osTimerNew((osTimerFunc_t)&TimerRTC_Callback, osTimerPeriodic, &exec1, NULL);
  
//  osTimerStart(tim_idRTC, 180000U); 
  return NULL;
}
/*----------------------------------------------------------------------------
*      Thread_RTC
 *---------------------------------------------------------------------------*/

int init_Thread_RTC (void) {

tid_ThRTC = osThreadNew(Thread_RTC, NULL, &Thread_RTC_attr);
if (tid_ThRTC == NULL) {
  return(-1);
}

return(0);
}

void Thread_RTC (void *argument) {
  Init_RTC();
  HAL_RTC_MspInit(&RtcHandle);
  Init_MsgQueue_Date ();

  struct tm *ptr_ts;
  time_t sys_time=(time_t)1774259400;
  ptr_ts=localtime(&sys_time);
  ts=*ptr_ts;
  RTC_CalendarConfig(ts);
  
  init_TimerRTC ();
  
  while (1) {
    RTC_CalendarShow(msg_env_web.BufHour, msg_env_web.BufDate);
    printf("%s\n%s\n\n", msg_env_web.BufHour, msg_env_web.BufDate);
    osMessageQueuePut(mid_Msg_Date, &msg_env_web, 0, 0);
        
    osDelay(1000);
    osThreadYield();
  }
}
