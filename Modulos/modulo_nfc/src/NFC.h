#ifndef __NFC_H
#define __NFC_H

#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "cmsis_os2.h"
#define SPI_NFC_fclk 500000


#define RESERVED00 0x00
#define COMMANDREG 0x01
#define COMIENREG 0x02
#define DIVLENREG 0x03
#define     COMIRQREG 0x04
#define     DIVIRQREG 0x05
#define     ERRORREG 0x06
#define     STATUS1REG            0x07
#define     STATUS2REG            0x08
#define     FIFODATAREG           0x09
#define     FIFOLEVELREG          0x0A
#define     WATERLEVELREG         0x0B
#define     CONTROLREG            0x0C
#define     BITFRAMINGREG         0x0D
#define     COLLREG               0x0E
#define     RESERVED01            0x0F
#define     RESERVED10            0x10
#define     MODEREG               0x11
#define     TXMODEREG             0x12
#define     RXMODEREG             0x13
#define     TXCONTROLREG          0x14
#define     TXAUTOREG             0x15
#define     TXSELREG              0x16
#define     RXSELREG              0x17
#define     RXTHRESHOLDREG        0x18
#define     DEMODREG              0x19
#define     RESERVED11            0x1A
#define     RESERVED12            0x1B
#define     MIFAREREG             0x1C
#define     RESERVED13            0x1D
#define     RESERVED14            0x1E
#define     SERIALSPEEDREG        0x1F
#define     RESERVED20            0x20
#define     CRCRESULTREGM         0x21
#define     CRCRESULTREGL         0x22
#define     RESERVED21            0x23
#define     MODWIDTHREG           0x24
#define     RESERVED22            0x25
#define     RFCFGREG              0x26
#define     GSNREG                0x27
#define     CWGSPREG              0x28
#define     MODGSPREG             0x29
#define     TMODEREG              0x2A
#define     TPRESCALERREG         0x2B
#define     TRELOADREGH           0x2C
#define     TRELOADREGL           0x2D
#define     TCOUNTERVALUEREGH     0x2E
#define     TCOUNTERVALUEREGL     0x2F
#define     RESERVED30            0x30
#define     TESTSEL1REG           0x31
#define     TESTSEL2REG           0x32
#define     TESTPINENREG          0x33
#define     TESTPINVALUEREG       0x34
#define     TESTBUSREG            0x35
#define     AUTOTESTREG           0x36
#define     VERSIONREG            0x37
#define     ANALOGTESTREG         0x38
#define     TESTDAC1REG           0x39
#define     TESTDAC2REG           0x3A
#define     TESTADCREG            0x3B
#define     RESERVED31            0x3C
#define     RESERVED32            0x3D
#define     RESERVED33            0x3E
#define     RESERVED34            0x3F

//COMANDOS MFRC5222

#define IDLE 0x00
#define TRASMIT 0x04  //trasmite FIFO a micro

#define RECIVE 0x08
#define TRANSCEIVE 0x0C //Trasmitir FIFO a tarjeta por la antena
#define SOFTRESERT 0x0F


//COMANDO TARJETA
#define REQA 0x26
#define UID_1 0x93
#define UID_2 0x20

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
void NFC_read_UID(uint8_t *buff);

#endif
