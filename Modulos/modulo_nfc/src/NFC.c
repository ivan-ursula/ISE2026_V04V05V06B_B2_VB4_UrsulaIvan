#include "NFC.h"

extern ARM_DRIVER_SPI Driver_SPI2 ;
ARM_DRIVER_SPI * SPI_NFC=&Driver_SPI2;
/**********************

FUNCIONES DE HILO

***********************/
osThreadId_t th_NFC;

void thread_NFC (void *argument);
int init_thNFC(void){
  th_NFC = osThreadNew(thread_NFC, NULL, NULL);
  if (th_NFC == NULL) {
    return(-1);
  }
 
  return(0);
}
void thread_NFC(void *argument){
  NFC_init_SPI();
	NFC_init();
  while(1){
    osThreadFlagsWait(0X2,osFlagsWaitAny,osWaitForever);
    
		osDelay(200);
  }
  
}

/**********************

FUNCIONES DE NFC

***********************/
void NFC_init_SPI(void){
	
	
  GPIO_InitTypeDef gpio;
	
	//iniciar interrupcion
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	gpio.Mode=GPIO_MODE_IT_RISING;
	gpio.Pull=GPIO_NOPULL;
	gpio.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	
	gpio.Pin=GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB,&gpio);
  
  SPI_NFC->Initialize(NFC_SPI_callback);
  SPI_NFC->PowerControl(ARM_POWER_FULL);
  SPI_NFC->Control(ARM_SPI_MODE_MASTER| ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8),SPI_NFC_fclk);
}

void NFC_init(void){
  NFC_init_SPI();
  
  NFC_wr_register(COMIENREG,0x20);
  NFC_wr_register(DIVIENREG,0x80);
	
	NFC_wr_register(TXMODEREG,0x00);
	NFC_wr_register(RXMODEREG,0x00);
	
	NFC_wr_register(MODWIDTHREG,0x26);
	
	NFC_wr_register(TMODEREG,0x80);
	NFC_wr_register(TPRESCALERREG,0x49);
	NFC_wr_register(TRELOADREGH,0x03);
	NFC_wr_register(TRELOADREGL,0xE8);
	
	NFC_wr_register(TXASKREG,0x40);
	NFC_wr_register(MODEREG,0x3D);
	
	NFC_wr_register(TXCONTROLREG,0X03);
	
}


uint8_t NFC_read_register(uint8_t reg){
	reg=0x80|reg;
  SPI_NFC->Send(&reg,1);
  osThreadFlagsWait(0x1,osFlagsWaitAll,osWaitForever);
  // esperar a que termine incluir
  SPI_NFC->Receive(&reg,1);
  osThreadFlagsWait(0x1,osFlagsWaitAll,osWaitForever);
  return reg;
}

void NFC_wr_register(uint8_t reg,uint8_t data){
  uint8_t msg[2];
  msg[0]=reg;
  msg[1]=data;
  SPI_NFC->Send(msg,2);
  osThreadFlagsWait(0x1,osFlagsWaitAll,osWaitForever);
  
  
}


void NFC_SPI_callback(uint32_t evn){
  
  switch (evn){
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
      
      osThreadFlagsSet(th_NFC,1);
      break;
    
    
    
  }
  
}
 void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

}

void  HAL_GPIO_EXTI_Callback(uint16_t pin){
	
	if(pin==GPIO_PIN_10){
		osThreadFlagsSet(th_NFC,2);
		NFC_wr_register(COMIRQREG,0x7f);
	}


}
