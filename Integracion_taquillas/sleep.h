#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_eth.h"
#include "cmsis_os2.h"

#define LAN8742A_PHY_ADDRESS 0x00U
#ifndef __SLEEP_H
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_UNDERDRIVE_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
/* Uncomment the corresponding line to select the STM32F4xx Low Power mode */
#define SLEEP_MODE
//#define STOP_MODE 
//#define STOP_UNDERDRIVE_MODE 
//#define STANDBY_MODE
/* #define STANDBY_RTC_MODE */
/* #define STANDBY_RTC_BKPSRAM_MODE */
#endif

#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_UNDERDRIVE_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
 #error "Please select first the target STM32F4xx Low Power mode to be measured (in stm32f4xx_lp_modes.h file)"
#endif

void SleepMode_Measure(void);
void StopMode_Measure(void);
void StopUnderDriveMode_Measure(void);
void StandbyMode_Measure(void);
void StandbyRTCMode_Measure(void);
void StandbyRTCBKPSRAMMode_Measure(void);
void ETH_PhyEnterPowerDownMode(void);
void ETH_PhyExitFromPowerDownMode(void);
#endif
