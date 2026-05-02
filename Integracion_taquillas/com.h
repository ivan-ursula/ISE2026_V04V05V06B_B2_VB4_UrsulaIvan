#ifndef __COM_H
#define __COM_H

#include "stm32f4xx_hal.h"


#define baudrate 9600
//principio y fin de trrama
#define SOH 0X01
#define EOT 0XFE
#define HORA 0X20

//CMD para acelelometro
#define AX_R 0X25
#define AY_R 0X26
#define AZ_R 0X27

#define A_ALL 0X55
#define CLEAR_ALL 0X60

// Respuestas 

#define RESP_HORA 0XDF
#define RESP_AX_R 0XDA
#define RESP_AY_R 0XD9
#define RESP_AZ_R 0XD9
#define RESP_A_ALL 0XAF
#define RESP_CLEAR_ALL 0X60

typedef struct{
	uint8_t cmd;
	uint8_t buff[50];
	uint8_t length;
	
} ComData_t;


#endif

