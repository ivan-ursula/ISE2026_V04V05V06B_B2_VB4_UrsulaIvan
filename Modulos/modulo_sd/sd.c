#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "sd.h"
#include "stdio.h"
#include "string.h"
#include "Driver_SPI.h"

/* Inicializacion del driver SPI */
ARM_SPI_STATUS status;
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

osThreadId_t th_id_SD;

void th_SD(void *argument);                   
static void Init_Pins_SD(void);
static void mySPI_Callback(uint32_t event);
static void Init_SPI_SD(void);
static uint8_t Init_SD(void);
static uint8_t SD_SPI_TxRx(uint8_t tx);

int Init_th_SD (void) 
{
	th_id_SD = osThreadNew(th_SD, NULL, NULL);
 
  return(0);
}
 uint8_t rx; 
void th_SD (void *argument) 
{
	rx = Init_SD();
  while (1)
	{
		osThreadYield();
  }
}

static void Init_Pins_SD(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOD_CLK_ENABLE();

	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = CS;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_SET);
	
  
}
static void mySPI_Callback(uint32_t event)
{
	if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE)
		osThreadFlagsSet(th_id_SD, FLAG_SEND_SD);
}
static void Init_SPI_SD(void)
{
	SPIdrv->Initialize(mySPI_Callback);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | 
  ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 400000); //200 MHz
}
static uint8_t Init_SD(void)
{
	uint8_t rx = 0xFF;
	Init_Pins_SD();
	Init_SPI_SD();
	
	SD_CS_HIGH();
	osDelay(10);
	SD_CS_LOW();
	
	SD_SPI_TxRx(0xFF);
	SD_SPI_TxRx(0x40);  // CMD0
  SD_SPI_TxRx(0x00);
  SD_SPI_TxRx(0x00);
  SD_SPI_TxRx(0x00);
	SD_SPI_TxRx(0x00);
  SD_SPI_TxRx(0x95);
	SD_SPI_TxRx(0xFF);
	osDelay(10);
	rx = SD_SPI_TxRx(0xFF);
	SD_CS_HIGH();
	SD_SPI_TxRx(0xFF);
	return rx;
	
}