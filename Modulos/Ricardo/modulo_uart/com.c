#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "com.h"
#include "Driver_USART.h"

osThreadId_t thcom_Tx;
osThreadId_t thcom_Rx;

osMessageQueueId_t qCom_Tx;
osMessageQueueId_t qCom_Rx;

extern ARM_DRIVER_USART Driver_USART3;
ARM_DRIVER_USART * uart=&Driver_USART3;

void th_com_Tx(void *argument);                   
void th_com_Rx(void *argument);
// uart funciones
void uart_send_trama(ComData_t * msg);
void uart_callback(uint32_t event);
void procesar_trama(uint8_t* b,uint32_t length);
static void init_uart(void);


int init_thcom (void) {
 
  thcom_Tx = osThreadNew(th_com_Tx, NULL, NULL);
	thcom_Rx = osThreadNew(th_com_Rx, NULL, NULL);
	
	qCom_Tx = osMessageQueueNew(10,sizeof(ComData_t),NULL);
	qCom_Rx = osMessageQueueNew(10,sizeof(ComData_t),NULL);
	
	init_uart();
  if (thcom_Tx == NULL || thcom_Rx == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_com_Tx (void *argument) {
 ComData_t msg;
 
  uint8_t buff[50];
	//mirar si con el bufer creandolo asi funciona
  while (1) {

		
		osMessageQueueGet(qCom_Tx,&msg,NULL,osWaitForever);
		
		//uint8_t* buff=(uint8_t*)malloc(msg.length+4);
		buff[0]=SOH;
		buff[1]=msg.cmd;
		buff[2]=msg.length+4;
		
		for(int i=0 ; i < msg.length; i++){
			buff[i+3] = msg.buff[i];
			
		}
		
		buff[msg.length+3]=EOT;
		
		uart->Send(buff,buff[2]);
		osThreadFlagsWait(0x1,osFlagsWaitAll,osWaitForever);
		
		//free(buff);
    osThreadYield();
  }
}

void th_com_Rx(void *argument){
	uint8_t data;
	uint8_t data_buff[50];
	uint8_t flag_recibe=0;
	uint32_t count_buff=0;
	
	while(1){
		uart->Receive(&data,1);
		osThreadFlagsWait(0x1,osFlagsWaitAll,osWaitForever);
		
		
		if(!flag_recibe){
			if(data & SOH){
				flag_recibe=1;
				data_buff[count_buff]=SOH;
				count_buff++;
				
			}	
		}else{
			if(data != EOT){
				data_buff[count_buff]=data;
				count_buff++;
				
			}else{
				data_buff[count_buff]=data;
				count_buff++;
				flag_recibe=0;
				
				if(count_buff/data_buff[2]!=1){
					for(int i=0 ; i<count_buff; i++){
						data_buff[i]=0;					
					}
					count_buff=0;
					
				}else{
					procesar_trama(data_buff,count_buff);
					count_buff=0;
				}			
			}
		}			
		osThreadYield();
		
	}	
}


static void init_uart(void){
		GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
//  GPIO_InitStruct.Pin = GPIO_PIN_10 |GPIO_PIN_11;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//		
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	uart->Initialize(uart_callback);
	uart->PowerControl(ARM_POWER_FULL);
	uart->Control(ARM_USART_MODE_ASYNCHRONOUS |
                ARM_USART_DATA_BITS_8 			|
                ARM_USART_PARITY_NONE 			|
                ARM_USART_STOP_BITS_1 			|
                ARM_USART_FLOW_CONTROL_NONE, baudrate);
	
	uart->Control(ARM_USART_CONTROL_TX,1);
	uart->Control(ARM_USART_CONTROL_RX,1);
}


void uart_callback(uint32_t event){
	uint32_t mask;
  mask = 
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
	
	if(event & mask){
		osThreadFlagsSet(thcom_Tx,0x1);
	}	
	if(event & ARM_USART_EVENT_RECEIVE_COMPLETE ){
		osThreadFlagsSet(thcom_Rx,0x1);
	}		
}

void procesar_trama(uint8_t* b,uint32_t length){
	ComData_t aux;
	
	aux.cmd=b[1];
	aux.length= length-4;
	
	for(int i=0;i<aux.length;i++){
		aux.buff[i]=b[i+3];
		
	}
	
	osMessageQueuePut(qCom_Rx,&aux,0,0);
		
}

