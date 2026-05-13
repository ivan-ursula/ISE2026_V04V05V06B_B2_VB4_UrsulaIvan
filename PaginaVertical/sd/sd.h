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
		uint32_t id_tarjeta;
    uint8_t taquilla;   // 0: Ninguna, 1: Taq 1, 2: Taq 2
    char nombre_trabajador[20];
} TRABAJADOR_t;

int Init_th_SD (void);

void alarm_write(char *date, char *time, char *texto);
void user_write(char *texto);
int alarm_read_page(uint16_t pag_actual, char buffer_dest[10][90]);

int user_add(TRABAJADOR_t trabajador[]);
int user_edit(const char *nfc_id_target, const char *new_user);
int user_delete(const char *nfc_id_target);

#endif

