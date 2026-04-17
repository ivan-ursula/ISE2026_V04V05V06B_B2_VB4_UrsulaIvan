#ifndef __LCD_H
#define __LCD_H

#include "Driver_SPI.h"  
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define FLAG_SEND_LCD 0x01

#define MSGQUEUE_OBJECTS_LCD 2
#define BUFLCD_MAX 128
#define CS GPIO_PIN_14
#define RST GPIO_PIN_6
#define AO GPIO_PIN_13

/* Message Queue LCD */
typedef struct {
	char BufLCD[BUFLCD_MAX];
  uint8_t linea; 
	int length;
} MSGQUEUE_OBJ_LCD;

/* Thread LCD */
extern osThreadId_t tid_ThLCD;
int init_Thread_LCD (void);
#endif
