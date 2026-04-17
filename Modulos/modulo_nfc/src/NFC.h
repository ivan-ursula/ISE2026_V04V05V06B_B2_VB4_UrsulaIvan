#ifndef __NFC_H
#define __NFC_H

#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "cmsis_os2.h"
#define SPI_NFC_fclk 500000

#define COMMANDREG 0x01
#define COMIENREG 0x02
#define DIVIENREG 0x03
#define COMIRQREG 0x04
#define ERRORREG 0x06
#define SATUS2REG 0x08
#define FIFODATAREG 0x0A
#define FIFOLEVELREG 0x0A
#define CONTROLREG 0x0C
#define BITFRAMINGREG 0x0D // enviar 0x80 despuess de cmd transceive para enviar a la tarjeta
#define MODEREG 0x11
#define TXMODEREG 0X12
#define RXMODEREG 0X13
#define TXCONTROLREG 0X14

#define TXASKREG 0X15
#define RXTHRESOHLDREG 0x18
#define DEMODREG 0x19
#define MODWIDTHREG 0X24

#define RFCFGREG 0x26
#define TMODEREG 0X2A

#define TPRESCALERREG 0X2B
#define TRELOADREGH 0X2C
#define TRELOADREGL 0X2D

//COMANDOS MFRC5222

#define IDLE 0x00
#define TRASMIT 0x04  //trasmite FIFO a micro

#define RECIVE 0x08
#define TRANSCEIVE 0x0C //Trasmitir FIFO a tarjeta por la antena
#define SOFTRESERT 0x0F


//COMANDO TARJETA
#define REQA 0x26
#define UID_1 0x93
#define UID_2 0x95



extern osThreadId_t th_NFC;

int init_thNFC(void);

void NFC_init_SPI(void);
void NFC_init(void);
void NFC_SPI_callback(uint32_t evn);
void NFC_wr_register(uint8_t reg,uint8_t data);
uint8_t NFC_read_register(uint8_t reg);
void NFC_resert_IRQ(void);
void NFC_antena_on(void);
void NFC_antena_off(void);

void NFC_set_mask(uint8_t reg,uint8_t mask);
void NFC_clear_mask(uint8_t reg,uint8_t mask);

int NFC_requestA(uint8_t *buff);

#endif
