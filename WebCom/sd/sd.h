#ifndef __SD_H
#define __SD_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define ALARM 0x01
#define NFC 0x02

/*Inicialización de la SD*/
int sd_init(void);

/*Gestión de la información de una NFC, taquilla asignada y trabajador*/
typedef struct {
		char id_tarjeta[9];
    uint8_t taquilla;   // 0: Ninguna, 1: Taq 1, 2: Taq 2
    char nombre_trabajador[20];
} TRABAJADOR_t;

int user_add(TRABAJADOR_t trabajador[]);
int user_read(TRABAJADOR_t trabajador[]);

/*Funciones de gestión de alarmas*/
void alarm_write(char *date, char *time, char *texto);
int alarm_read_page(uint16_t pag_actual, char buffer_dest[10][90]);

/*Función de búsqueda de NFC*/
int nfc_search(const char *nfc_id_target);

//variables servidor
extern uint8_t estado_taq;
extern float peso_taq1;
extern float peso_taq2;

extern float in_peso_taq1;
extern float in_peso_taq2;

extern uint8_t modo_func;

extern uint8_t hora_dorm_per;
extern uint8_t min_dorm_per;
extern uint8_t hora_desp_per;
extern uint8_t min_desp_per;
	
extern float intens;
extern uint8_t alerta;

//Variable de última NFC introducida
extern char id_nfc_new[9];

#endif

