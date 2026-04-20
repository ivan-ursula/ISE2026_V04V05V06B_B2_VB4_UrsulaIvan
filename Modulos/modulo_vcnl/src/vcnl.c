#include "vcnl.h"

extern ARM_DRIVER_I2C Driver_I2C1;

ARM_DRIVER_I2C *vcnl_i2c=&Driver_I2C1;

osThreadId_t th_VCNL;

void thread_VCNL(void *argument);

int init_thVCNL(void){
  th_VCNL=osThreadNew(thread_VCNL,NULL,NULL);
  if (th_VCNL == NULL) {
    return(-1);
  }
 
  return(0);
  
}
uint16_t id_vcncl;
uint32_t flag;
void thread_VCNL(void *argument){
  VCNL_init_I2C();
  while(1){
		VCNL_write_reg(ALS_THDH_H,0xf1f2);
		id_vcncl= VCNL_read_reg(ALS_THDH_H);
		flag=osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
    
		osDelay(1000);
		
  }
  
}

void VCNL_init_I2C(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  vcnl_i2c->Initialize(I2C_callback);
  vcnl_i2c->PowerControl(ARM_POWER_FULL);
  vcnl_i2c->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_STANDARD);
  vcnl_i2c->Control(ARM_I2C_BUS_CLEAR, 0);
	//osDelay(500);
}

void VCNL_init(void){
  VCNL_write_reg(ALS_CONF,0x0001);
	VCNL_write_reg(PS_CONF,0x0001);
	
  
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
  uint16_t aux;
  vcnl_i2c->MasterTransmit(ADDR,&reg,1,1);
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
  vcnl_i2c->MasterReceive(ADDR,data,2,0);
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
  
  return (uint16_t) ((data[1]<<8)|data[0]);
  
}
void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

}

void  HAL_GPIO_EXTI_Callback(uint16_t pin){
	
	if(pin==GPIO_PIN_10){
		
		
		osThreadFlagsSet(th_VCNL,2);
	}
}
uint32_t mask;
void I2C_callback(uint32_t event){
  
	mask = event;
	

	
	  if (event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) {
    /* Less data was transferred than requested */
  }
 
  if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
    /* Transfer or receive is finished */
		osThreadFlagsSet(th_VCNL,0x1);
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