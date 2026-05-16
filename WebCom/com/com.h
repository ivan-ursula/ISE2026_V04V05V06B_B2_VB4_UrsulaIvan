#ifndef __COM_H
#define __COM_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"

void init_uart(void);
void desinit_uart(void);

int init_thcom (void);
int init_thcomweb (void);

#define baudrate 9600
//principio y fin de trrama
#define SOH 0X01
#define EOT 0XFE
#define MIN_L 0x04

//CMD
#define HORA 0X20 //
#define DORMIR 0x21 //
#define ALARM_COM 0x25 //
#define LECTURA_NFC 0x70 //
#define LECTURA_PESO 0x80 //
#define LECTURA_TENSION 0x81 //
#define LECTURA_CORRIENTE 0x82 //


//respuestas
#define RESP_HORA 0XDF //
#define RESP_DORMIR 0xDE //
#define RESP_ALARM_COM 0xDA //
#define RESP_LECTURA_PESO 0x7F //
#define RESP_LECTURA_TENSION 0x7E //
#define RESP_LECTURA_CORRIENTE 0x7D //
#define RESP_ESTADO_TAQUILLA 0x8F //


typedef struct{
	uint8_t cmd;
	char buff[50];
	uint8_t length;
	
} ComData_t;


#endif

