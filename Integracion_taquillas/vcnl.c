#include "vcnl.h"
#include <stdio.h>

extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C *vcnl_i2c=&Driver_I2C1;

osThreadId_t th_id_VCNL;
void thread_VCNL(void *argument);

uint16_t id_vcnl,in_flag;
uint32_t flag;

int init_thVCNL(void){
  th_id_VCNL = osThreadNew(thread_VCNL,NULL,NULL);
  if (th_id_VCNL == NULL) {
    return(-1);
  }
 
  return(0);
  
}
void thread_VCNL(void *argument){
  VCNL_init_I2C();
	VCNL_init();
	
  while(1){
		
		id_vcnl = VCNL_read_reg(PS_DATA);
    flag = osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);

    if (flag == 0x02) {
				in_flag = VCNL_read_reg(INT_FLAG);

			  //osDelay(1000);

    }

		
		osThreadYield();
  }
  
}

void VCNL_init_I2C(void)
{
	vcnl_i2c->Initialize(I2C_callback);
  vcnl_i2c->PowerControl(ARM_POWER_FULL);
  vcnl_i2c->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_STANDARD);
  vcnl_i2c->Control(ARM_I2C_BUS_CLEAR, 0);
	
	INT_init();
	
}
void INT_init (void)
{
	GPIO_InitTypeDef gpio;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpio.Pin = INT_PIN;
	gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	
	HAL_GPIO_Init(GPIOB ,&gpio);
}

void VCNL_init(void){
	
  VCNL_write_reg(ALS_CONF,0x0001);
	VCNL_write_reg(PS_THDL_L,0x0004);
	VCNL_write_reg(PS_THDL_H,0x000A);
	VCNL_write_reg(PS_CONF,0x03FE);//1=cerca 2=lejos        0x30 
	osDelay(200);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // para evitar falsos positivos durante la configuracion
  
}

void VCNL_write_reg(uint8_t reg, uint16_t data){
  uint8_t buff[3];
	buff[0]=reg;
  buff[1]= (uint8_t)(data&0x00FF);
  buff[2]=(uint8_t)(data>>8);
  vcnl_i2c->MasterTransmit(ADDR,buff,3,0);
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
  
  
}
uint16_t VCNL_read_reg(uint8_t reg){
  uint8_t data[2];
  //uint16_t aux;
  vcnl_i2c->MasterTransmit(ADDR,&reg,1,1);
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
  vcnl_i2c->MasterReceive(ADDR,data,2,0);
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
  
  return (uint16_t) ((data[1]<<8)|data[0]);
  
}

uint32_t mask;
void I2C_callback(uint32_t event)
{
  
	mask = event;
	

	
	  if (event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) {
    /* Less data was transferred than requested */
  }
 
  if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
    /* Transfer or receive is finished */
		osThreadFlagsSet(th_id_VCNL,0x1);
  }
 
  if (event & ARM_I2C_EVENT_ADDRESS_NACK) {
    /* Slave address was not acknowledged */
  }
 
  if (event & ARM_I2C_EVENT_ARBITRATION_LOST) {
    /* Master lost bus arbitration */
  }
 
  if (event & ARM_I2C_EVENT_BUS_ERROR) {
    /* Invalid start/stop position detected */
  }
 
  if (event & ARM_I2C_EVENT_BUS_CLEAR) {
    /* Bus clear operation completed */
  }
 
  if (event & ARM_I2C_EVENT_GENERAL_CALL) {
    /* Slave was addressed with a general call address */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_RECEIVE) {
    /* Slave addressed as receiver but SlaveReceive operation is not started */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_TRANSMIT) {
    /* Slave addressed as transmitter but SlaveTransmit operation is not started */
  }

  
}
