#ifndef __VCNL_H
#define __VCNL_H
#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_I2C.h"
#include "cmsis_os2.h"

#define ALS_CONF        0x00
#define ALS_THDH_H      0x01
#define ALS_THDH_L      0x02
#define PS_CONF         0x03
#define PS_CONF_3_MS    0x04
#define PS_CANC         0x05
#define PS_THDL_L       0x06
#define PS_THDL_H       0x07
#define PS_DATA         0x08
#define ALS_DATA        0x09
#define WHITE_DATA      0x0A
#define INT_FLAG        0x0B
#define ID              0x0C

#define INT_PIN GPIO_PIN_10

#define VCNL_FLAG_PS_EVENT  0x02   /* lectura por interrupción/alarma */
#define VCNL_FLAG_REINIT    0x04   /* reinicializar tras bajo consumo */

#define ADDR            0x60
extern osThreadId_t th_id_VCNL;
extern ARM_DRIVER_I2C *vcnl_i2c;
int init_thVCNL(void);
void VCNL_init(void);
void VCNL_init_I2C(void);
void INT_init (void);
void I2C_callback(uint32_t event);

void VCNL_write_reg(uint8_t reg, uint16_t data);
uint16_t VCNL_read_reg(uint8_t reg);

#endif
