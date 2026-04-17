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
int8_t status_mcr;
void thread_NFC (void *argument);
	int init_thNFC(void){
  th_NFC = osThreadNew(thread_NFC, NULL, NULL);
  if (th_NFC == NULL) {
    return(-1);
  }
 
  return(0);
}
  uint8_t version;

void thread_NFC(void *argument){
  NFC_init_SPI();
	NFC_init();
  while(1){
    
    version=NFC_read_register(0x37);

		//NFC_resert_IRQ();
    status_mcr=NFC_requestA(buff);
		
		if(status_mcr !=0){
      version=1;
      
    }
		osDelay(100);
		
		
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
  
  osDelay(50);
	
	
	NFC_wr_register(COMMANDREG,SOFTRESERT);
	
	NFC_wr_register(TMODEREG,0x8D); //90//80
	NFC_wr_register(TPRESCALERREG,0x3E); //timer 500 us	
	NFC_wr_register(TRELOADREGL,30); //3e8
	NFC_wr_register(TRELOADREGH,0); //contador empieza en 250	

	NFC_wr_register(TXASKREG,0x40);

	NFC_wr_register(MODEREG,0x3D);

	
	
	
	NFC_antena_on();

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
	NFC_wr_register(TXCONTROLREG,(rg|0x03));
}

void NFC_antena_off(void){
	uint8_t rg;
	rg=NFC_read_register(TXCONTROLREG);
	NFC_wr_register(TXCONTROLREG,(rg&(~0x03)));
	
}

void NFC_SPI_callback(uint32_t evn){
  
  switch (evn){
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
      
      osThreadFlagsSet(th_NFC,1);
      break;
        case ARM_SPI_EVENT_DATA_LOST:
					
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
			
        break;
    
    
    
  }
  
}
uint8_t err;
uint8_t fifodata_lh;
uint8_t status;
uint8_t irq_status;
int NFC_requestA(uint8_t *buff){
	uint8_t irq_en=0x77;
	uint8_t irq=0x30;
	uint8_t aux;
	 uint8_t n,i;
  uint8_t trasmit_fin;
	
	NFC_wr_register(BITFRAMINGREG,0x07);
	
	NFC_wr_register(COMIENREG,irq_en|0x80);
	
	NFC_clear_mask(COMIRQREG,0x80);
	NFC_set_mask(FIFOLEVELREG,0x80);
	NFC_wr_register(COMMANDREG,IDLE);
	
	NFC_wr_register(FIFODATAREG,REQA);
	NFC_wr_register(COMMANDREG,TRANSCEIVE);
	NFC_set_mask(BITFRAMINGREG,0x80);	
	
	  i = 0xFFFF;
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = NFC_read_register( COMIRQREG );
    i--;
  }
  while ( i && !(n & 0x01) && !( n & irq ) );
	
	NFC_clear_mask(BITFRAMINGREG,0x80);
  if (i != 0)
  {
    if( !( NFC_read_register( ERRORREG ) & 0x1B ) ) //BufferOvfl Collerr CRCErr ProtecolErr
    {
     // _status = MI_OK;
      if ( n & irq_en & 0x01 )
      {
       // _status = MI_NOTAGERR;       //??
      }
        n = NFC_read_register( FIFOLEVELREG );
        trasmit_fin=(NFC_read_register(CONTROLREG)&0x07);
      
        for(int b=0;b<n;b++){
					buff[b]=NFC_read_register(FIFODATAREG);
				
				}

				return n;
  
      
    }
    else
    {
      return 0;
    }
  }	
	
	return 0;
}
void NFC_set_mask(uint8_t reg,uint8_t mask){
	uint8_t aux;
	aux=NFC_read_register(reg);
	NFC_wr_register(reg,aux|mask);
}
void NFC_clear_mask(uint8_t reg,uint8_t mask){
	uint8_t aux;
	aux=NFC_read_register(reg);
	NFC_wr_register(reg,aux|(~mask));
}
void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

}

void  HAL_GPIO_EXTI_Callback(uint16_t pin){
	
	if(pin==GPIO_PIN_10){
		
		
		osThreadFlagsSet(th_NFC,2);
	}
}

