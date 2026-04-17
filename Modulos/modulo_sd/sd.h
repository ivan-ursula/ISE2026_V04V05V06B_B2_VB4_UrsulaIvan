#ifndef __SD_H
#define __SD_H

#include "stm32f4xx_hal.h"

#define FLAG_SEND_SD 0x01

#define CS GPIO_PIN_14

#define SD_CS_LOW()   HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_RESET)
#define SD_CS_HIGH()  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_SET)

#endif

