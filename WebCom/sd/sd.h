#ifndef __SD_H
#define __SD_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define BUFSD_MAX 128
#define MSGQUEUE_OBJECTS_SD 1

#define ALARM 0x01
#define NFC 0x02

typedef struct 
{
	char BufSD[BUFSD_MAX];
	
} MSGQUEUE_OBJ_SD;
extern osMessageQueueId_t mid_Msg_SD;

typedef struct {
		char id_tarjeta[9];
    uint8_t taquilla;   // 0: Ninguna, 1: Taq 1, 2: Taq 2
    char nombre_trabajador[20];
} TRABAJADOR_t;

int Init_th_SD (void);

void alarm_write(char *date, char *time, char *texto);
void user_write(char *texto);
int alarm_read_page(uint16_t pag_actual, char buffer_dest[10][90]);
int nfc_search(const char *nfc_id_target);

int user_add(TRABAJADOR_t trabajador[]);

//variables server
extern uint8_t estado_taq; //Indica que taquilla está abierta o cerrada
extern float peso_taq1;
extern float peso_taq2;

extern uint8_t modo_func;

extern uint8_t hora_desp;
extern uint8_t min_desp;
extern char fecha_desp[11];

extern uint8_t hora_dorm;
extern uint8_t min_dorm;
extern char fecha_dorm[11];
	
extern float tens;
extern float intens;
extern float bat;
extern uint8_t alerta;

extern char id_nfc_new[9];

#endif

