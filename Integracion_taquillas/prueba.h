#ifndef __PRUEBA_H
#define __PRUEBA_H
#include <stdio.h>
#include <string.h>
#include "NFC.h"
#include "com.h"
#include "RTC.h"
#include "adc.h"
#include "sleep.h"
typedef enum{
	ACTIVO,
	BAJO_CONSUMO,
	ALARMA
	
} estados_t;
void estado_Activo(void);
void comp_cmd(ComData_t com_data);
void TimerRTC_Callback(void const *arg);

#endif
