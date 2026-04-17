#include "LCD.h"
#include "Arial12x12.h"

/* Inicializacion de los pines */
GPIO_InitTypeDef GPIO_InitStruct;

/* Inicializacion del driver SPI */
ARM_SPI_STATUS status;
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

/* Variables del LCD */
static unsigned char bufferLCD[512];
static uint16_t position = 0;


/*Funciones*/
static void Pins_LCD_Init(void);
static void LCD_Reset(void);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_update(void);
static void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
static void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
static void symbolToLocalBuffer(uint8_t line,uint8_t symbol);
static void Init_LCD(void);
static void LCD_Clean(void);
static void LCD_Clean_L1(void);
static void LCD_Clean_L2(void);


/* Message */
MSGQUEUE_OBJ_LCD msg_LCD_leido;
osMessageQueueId_t mid_Msg_LCD;
osStatus_t status_LCD_estado;
int Init_MsgQueue_LCD (void);

/* Thread_LCD*/
osThreadId_t tid_ThLCD;

void Thread_LCD (void *argument);

/* Stack */
const osThreadAttr_t Thread_LCD_attr = {.stack_size = 1024};

/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/

int Init_MsgQueue_LCD (void) {

  mid_Msg_LCD = osMessageQueueNew(MSGQUEUE_OBJECTS_LCD, sizeof(MSGQUEUE_OBJ_LCD), NULL);
  if (mid_Msg_LCD == NULL) {
    return(-1);
  }
	
	return(0);
}

/*----------------------------------------------------------------------------
*      Thread_LCD
 *---------------------------------------------------------------------------*/
int init_Thread_LCD (void) {

tid_ThLCD = osThreadNew(Thread_LCD, NULL, &Thread_LCD_attr);
if (tid_ThLCD == NULL) {
  return(-1);
}

return(0);
}
void Thread_LCD (void *argument) {
	Init_LCD();
	Init_MsgQueue_LCD();
	
	while (1) {

		osStatus_t status_LCD = osMessageQueueGet(mid_Msg_LCD, &msg_LCD_leido, NULL, osWaitForever);
    if(msg_LCD_leido.linea == 1){
      LCD_Clean_L1();
    }else {
      LCD_Clean_L2();
    }
		if(status_LCD == osOK){
			for(int i = 0; i < msg_LCD_leido.length; i++){
				symbolToLocalBuffer(msg_LCD_leido.linea, msg_LCD_leido.BufLCD[i]);
			}
			LCD_update();
		}
  osThreadYield();
	}
}
  

/*----------------------------------------------------------------------------
 *      Definicion de todas las funciones usadas para el LCD
 *---------------------------------------------------------------------------*/
static void Pins_LCD_Init(){
  
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = CS;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_SET);
  

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = RST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(GPIOA, RST, GPIO_PIN_SET);
  

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = AO;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(GPIOF, AO, GPIO_PIN_SET);
  
}

static void mySPI_Callback(uint32_t event){
    if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE)
        osThreadFlagsSet(tid_ThLCD, FLAG_SEND_LCD);
}

static void LCD_Reset(){ //Aqui puede dar problemas
	// Inicializacion y configuracion del driver SPI 
  SPIdrv->Initialize(mySPI_Callback);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | 
  ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
	
	osDelay(1);
  HAL_GPIO_WritePin(GPIOA, RST, GPIO_PIN_RESET);
	osDelay(5);
  HAL_GPIO_WritePin(GPIOA, RST, GPIO_PIN_SET);
	osDelay(1);
}

////////////////////////////LIMPIAR LA PANTALLA/////////////////////////////////////////
void LCD_Clean(void){
  memset(bufferLCD, 0, 512U);
  LCD_update();
}

void LCD_Clean_L1(void){
  for(int i = 0; i < 256; i++){
    bufferLCD[i] = 0;
  }
}

void LCD_Clean_L2(void){
  for(int i = 256; i < 512; i++){
    bufferLCD[i] = 0;
  }
}


static void LCD_wr_data(unsigned char data){
	
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_RESET);
	
  HAL_GPIO_WritePin(GPIOF, AO, GPIO_PIN_SET);
	
	SPIdrv->Send(&data, sizeof(data));
  
  osThreadFlagsWait(FLAG_SEND_LCD, osFlagsWaitAny, osWaitForever);
  
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_SET);
}


static void LCD_wr_cmd(unsigned char cmd){
	
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_RESET);
	
  HAL_GPIO_WritePin(GPIOF, AO, GPIO_PIN_RESET);
  
  SPIdrv->Send(&cmd, sizeof(cmd));
  
  osThreadFlagsWait(FLAG_SEND_LCD, osFlagsWaitAny, osWaitForever);
  
  HAL_GPIO_WritePin(GPIOD, CS, GPIO_PIN_SET);
}
static void Init_LCD(void){ // Aqui tambien puede dar problemas
	
	Pins_LCD_Init();
	
	LCD_Reset();
	
  LCD_wr_cmd(0xAE); 
  LCD_wr_cmd(0xA2);
  LCD_wr_cmd(0xA0);
  LCD_wr_cmd(0xC8);
  LCD_wr_cmd(0x22);
  LCD_wr_cmd(0x2F);
  LCD_wr_cmd(0x40);
  LCD_wr_cmd(0xAF);
  LCD_wr_cmd(0x81);
  LCD_wr_cmd(0x17);
  LCD_wr_cmd(0xA4);
  LCD_wr_cmd(0xA6);
  
  LCD_Clean();
}
static void LCD_update(void){
  int i;
  
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0); // Página 0

  for(i=0;i<128;i++){
    LCD_wr_data(bufferLCD[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1); // Página 1

  for(i=128;i<256;i++){
    LCD_wr_data(bufferLCD[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB2); //Página 2
  
  for(i=256;i<384;i++){
    LCD_wr_data(bufferLCD[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB3); // Pagina 3

  for(i=384;i<512;i++){
    LCD_wr_data(bufferLCD[i]);
  }
	position = 0;
	
}
static void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
  
  uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');
  
  for (i=0; i<12; i++){
    
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    
    bufferLCD[i+position]=value1;
    bufferLCD[i+128+position]=value2;
  }
  position=position+Arial12x12[offset];
	
}
static void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
  
  uint8_t i, value1, value2;
  uint16_t offset=0;
	
  offset=25*(symbol - ' ');
  
  for (i=0; i<12; i++){
    
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    
    bufferLCD[i+256+position]=value1;
    bufferLCD[i+384+position]=value2;
  }
  position=position+Arial12x12[offset];
}
static void symbolToLocalBuffer(uint8_t line,uint8_t symbol){
  if(line == 1)
		LCD_symbolToLocalBuffer_L1(symbol);
	else
    LCD_symbolToLocalBuffer_L2(symbol);
}

