#ifndef __COM_H
#define __COM_H

#include "stm32f4xx_hal.h"


#define baudrate 9600
//principio y fin de trrama
#define SOH 0X01
#define EOT 0XFE

//CMD pagina web (PW)
#define B_C    0X21
#define L_P    0x80
#define L_T    0X81
#define L_I    0x83

//CMD taquilla
#define H_F    0X20
#define ALR    0x25
#define NFC_R  0X70

// Respuestas 

#define RESP_B_C   0XDE
#define RESP_L_P   0X7F
#define RESP_L_T   0X7E
#define RESP_L_I   0X7D
#define RESP_H_F   0X
#define RESP_ALR   0XAF
#define RESP_NFC_R 0X60

typedef struct{
	uint8_t cmd;
	uint8_t buff[50];
	uint8_t length;
	
} ComData_t;


#endif

