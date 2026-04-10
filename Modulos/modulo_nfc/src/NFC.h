#ifndef __NFC_H
#define __NFC_H

#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "cmsis_os2.h"
#define SPI_NFC_fclk 100000

#define COMIENREG 0x02
#define DIVIENREG 0x03
#define COMIRQREG 0x04

#define MODEREG 0x11
#define TXMODEREG 0X12
#define RXMODEREG 0X13
#define TXCONTROLREG 0X14

#define TXASKREG 0X15

#define MODWIDTHREG 0X24
#define TMODEREG 0X2A

#define TPRESCALERREG 0X2B
#define TRELOADREGH 0X2C
#define TRELOADREGL 0X2D

extern osThreadId_t th_NFC;

int init_thNFC(void);

void NFC_init_SPI(void);
void NFC_init(void);
void NFC_SPI_callback(uint32_t evn);
void NFC_wr_register(uint8_t reg,uint8_t data);
uint8_t NFC_read_register(uint8_t reg);

#endif
