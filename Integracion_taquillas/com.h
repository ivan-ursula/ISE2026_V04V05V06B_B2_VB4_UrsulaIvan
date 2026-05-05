#ifndef __COM_H
#define __COM_H

#include "stm32f4xx_hal.h"


#define baudrate 9600
//principio y fin de trrama
#define SOH 0X01
#define EOT 0XFE


//CMD
#define HORA 0X20


#define DORMIR 0x21
#define LECTURA_NFC 0x70
#define LECTURA_PESO 0x80
#define LECTURA_TENSION 0x81
#define LECTURA_CORRIENTE 0x82
#define ESTADO_TAQUILLA 0X83


//respuestas
#define RESP_HORA 0XDF
#define RESP_LECTURA_PESO 0x7f
#define RESP_LECTURA_TENSION 0x7e
#define RESP_LECTURA_CORRIENTE 0x7d
#define RESP_ESTADO_TAQUILLA 0x7c



typedef struct{
	uint8_t cmd;
	uint8_t buff[50];
	uint8_t length;
	
} ComData_t;


#endif

