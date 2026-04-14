#include "NFC.h"

extern ARM_DRIVER_SPI Driver_SPI2 ;
ARM_DRIVER_SPI * SPI_NFC=&Driver_SPI2;
/**********************

FUNCIONES DE HILO

***********************/
osThreadId_t th_NFC;

uint32_t flag;
uint32_t num_byte;
uint8_t buff[30];

uint8_t datareg;
uint8_t status_mcr;
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
    flag=osThreadFlagsWait(0Xf,osFlagsWaitAny,500);
    
		//if(flag==2){
			//lectura de tarjeta
			status_mcr= NFC_read_register(COMIRQREG);
			if(status_mcr&1){
				NFC_wr_register(FIFOLEVELREG,0x80);
				NFC_wr_register(FIFODATAREG,REQA);
				NFC_wr_register(COMMANDREG,TRANSCEIVE);
				NFC_wr_register(BITFRAMINGREG,0x80);
				NFC_wr_register(CONTROLREG,0X40);
				//NFC_resert_IRQ();
			}
			if(NFC_read_register(COMIRQREG)==0x20){
				num_byte=(NFC_read_register(FIFOLEVELREG)&0x7F);
				for(int i=0;i<num_byte;i++){
					buff[i]=NFC_read_register(FIFODATAREG);
				
				}
			//	NFC_resert_IRQ();
		}			
	//}
		//NFC_resert_IRQ();
		status_mcr= NFC_read_register(COMIRQREG);
		osDelay(10);
		status_mcr= NFC_read_register(FIFODATAREG);
		osDelay(10);
		status_mcr= NFC_read_register(FIFODATAREG);
		osDelay(10);
		status_mcr= NFC_read_register(FIFODATAREG);
		osDelay(10);
		osThreadYield();
		
		
  }
  
}

/**********************

FUNCIONES DE NFC

***********************/

void NFC_init_SPI(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
  GPIO_InitTypeDef gpio;
	GPIO_InitTypeDef GPIO_InitStruct;
	//iniciar interrupcion
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	gpio.Mode=GPIO_MODE_IT_RISING;
	gpio.Pull=GPIO_PULLDOWN;
	gpio.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	
	gpio.Pin=GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB,&gpio);
	
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
  
  SPI_NFC->Initialize(NFC_SPI_callback);
  SPI_NFC->PowerControl(ARM_POWER_FULL);
  SPI_NFC->Control(ARM_SPI_MODE_MASTER| ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8),SPI_NFC_fclk);
	
}

void NFC_init(void){
  NFC_init_SPI();
  
	NFC_wr_register(COMMANDREG,SOFTRESERT);
	osDelay(10);
	NFC_wr_register(COMIRQREG,0x7F);
	
	NFC_wr_register(FIFOLEVELREG,0x80);
	
//  NFC_wr_register(COMIENREG,0x21);
//  NFC_wr_register(DIVIENREG,0x80);
//	datareg=NFC_read_register(COMIENREG);
	
//	NFC_wr_register(TXMODEREG,0x00);
//	NFC_wr_register(RXMODEREG,0x00);
	
//	NFC_wr_register(MODWIDTHREG,0x26);
	
	NFC_wr_register(TMODEREG,0x80); //90//80
	NFC_wr_register(TPRESCALERREG,0x49); //timer 500 us	
	NFC_wr_register(TRELOADREGH,0x00); //3e8
	NFC_wr_register(TRELOADREGL,0xFA); //contador empieza en 250
	
	NFC_wr_register(TXASKREG,0x40);
	NFC_wr_register(RFCFGREG,0x7f);
	NFC_wr_register(DEMODREG,0x4D);
	//NFC_wr_register(MODEREG,0x3D);
	
	
	
	NFC_antena_on();
	
//	NFC_wr_register(FIFOLEVELREG,0x80);
//	NFC_wr_register(FIFODATAREG,REQA);
//	NFC_wr_register(COMMANDREG,TRANSCEIVE);
//	NFC_wr_register(BITFRAMINGREG,0x80);
//	//NFC_wr_register(CONTROLREG,0X40);
//	NFC_wr_register(COMMANDREG,RECIVE);
//	//NFC_resert_IRQ();
	
}


uint8_t NFC_read_register(uint8_t reg){
	uint8_t rxmsg[2],txmsg[2];
	txmsg[0]=0x80|(reg<<1);
	txmsg[1]=0;
//  SPI_NFC->Send(&reg,1);
//  osThreadFlagsWait(0x1,osFlagsWaitAny,osWaitForever);
//  // esperar a que termine incluir
//  SPI_NFC->Receive(&msg,1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	SPI_NFC->Transfer(txmsg,rxmsg,2);
	
  osThreadFlagsWait(0x1,osFlagsWaitAny,osWaitForever);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
  return rxmsg[1];
}

void NFC_wr_register(uint8_t reg,uint8_t data){
  uint8_t msg[2];
	reg=(reg<<1);
  msg[0]=reg;
  msg[1]=data;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
  SPI_NFC->Send(msg,2);
  osThreadFlagsWait(0x1,osFlagsWaitAny,osWaitForever);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
  
}

void NFC_resert_IRQ(void){
	uint8_t a;
	a=NFC_read_register(COMIRQREG);
	NFC_wr_register(COMIRQREG,(0x7F&a));
}

void NFC_antena_on(void){
	uint8_t rg;
	rg=NFC_read_register(TXCONTROLREG);
	NFC_wr_register(TXCONTROLREG,(rg&(~0x03)));
}

void NFC_antena_off(void){
	uint8_t rg;
	rg=NFC_read_register(TXCONTROLREG);
	NFC_wr_register(TXCONTROLREG,(rg&0x03));
	
}
uint8_t err;
void NFC_SPI_callback(uint32_t evn){
  
  switch (evn){
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
      
      osThreadFlagsSet(th_NFC,1);
      break;
        case ARM_SPI_EVENT_DATA_LOST:
					err=1;
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
			err=2;
        break;
    
    
    
  }
  
}
void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

}

void  HAL_GPIO_EXTI_Callback(uint16_t pin){
	
	if(pin==GPIO_PIN_10){
		
		
		osThreadFlagsSet(th_NFC,2);
	}


}
