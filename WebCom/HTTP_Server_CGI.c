/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2018 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V6.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "rtc.h"
#include "sd/sd.h"
#include "com/com.h"


/*Al entrar una alarma del sensor de movimiento hacer un popup o un bloqueante
que no permita otra cosa que gestionar la alarma que acaba de llegar*/



#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

#define MAX_ALERTAS 15      //Número máximo de alertas
#define MAX_LEN_ALERTAS 30  //Longitud máxima de alerta

/*Flag para pedir medidas*/
extern osThreadId_t thweb_comRx;
extern osThreadId_t thweb_comTx;

/*Variables locales*/
static uint8_t ip_addr[NET_ADDR_IP6_LEN];
char msg_web[50];  //Mensaje string que se envía a la web

/*Variable para la visión de alertas*/
static uint16_t pag = 1;
static char buff_alertas [10][90];
static uint8_t num_alertas = 0;
static bool actualizar_cache = true; //Permite que solo cargue los datos de la SD al cambiar de página

/*Variables sobre la información de las taquillas*/
uint8_t estado_taq = 0;
float peso_taq1 = 0; //Peso actual 1
float peso_taq2 = 0; //Peso actual 2
float in_peso_taq1 = 0; //Peso inicial para calibración 1
float in_peso_taq2 = 0; //Peso inicial para calibración 2

/*Variables para los modos de funcionamiento del sistema*/
uint8_t modo_func = 1;
uint8_t alerta = 0;
float intens = 0;

/*Variables sobre la configuración del modo stop*/
uint8_t hora_dorm_per = 0;
uint8_t min_dorm_per = 0;
uint8_t hora_desp_per = 0;
uint8_t min_desp_per = 0;

/*Gestión de las horas del RTC*/
extern osMessageQueueId_t mid_Msg_Date;
MSGQUEUE_OBJ_DATE fecha_rec; //Fecha y hora recibida del RTC

/*Gestión para los datos de asignación NFC*/
static TRABAJADOR_t tabla_trabajadores[5];
static uint8_t carga_trabajadores = 0; //Permite la carga de datos de las NFC la primera vez

// My structure of CGI status variable.
typedef struct {
  uint8_t idx;
  uint8_t unused[3];
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  netIF_Option opt = netIF_OptionMAC_Address;
  int16_t     typ = 0;
  char var[40];

  do {
    // Loop through all the parameters
    qstr = netCGI_GetEnvVar (qstr, var, sizeof (var));
    // Check return string, 'qstr' now points to the next parameter

    switch (var[0]) {
      case 'i': // Local IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_Address;       }
        else               { opt = netIF_OptionIP6_StaticAddress; }
        break;

      case 'm': // Local network mask
        if (var[1] == '4') { opt = netIF_OptionIP4_SubnetMask; }
        break;

      case 'g': // Default gateway IP address
        if (var[1] == '4') { opt = netIF_OptionIP6_DefaultGateway; }
        else               { opt = netIF_OptionIP6_DefaultGateway; }
        break;

      case 'p': // Primary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_PrimaryDNS; }
        else               { opt = netIF_OptionIP6_PrimaryDNS; }
        break;

      case 's': // Secondary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_SecondaryDNS; }
        else               { opt = netIF_OptionIP6_SecondaryDNS; }
        break;
      
      default: var[0] = '\0'; break;
    }

    switch (var[1]) {
      case '4': typ = NET_ADDR_IP4; break;
      case '6': typ = NET_ADDR_IP6; break;

      default: var[0] = '\0'; break;
    }

    if ((var[0] != '\0') && (var[2] == '=')) {
      netIP_aton (&var[3], typ, ip_addr);
      // Set required option
      netIF_SetOption (NET_IF_CLASS_ETH, opt, ip_addr, sizeof(ip_addr));
    }
  } while (qstr);
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40];

  if (code != 0) {
    // Ignore all other codes
    return;
  }

  estado_taq = 0;
  
  if (len > 0 && data != NULL) {
    // No data or all items (radio, checkbox) are off
    do {
      // Parse all parameters
      data = netCGI_GetEnvVar (data, var, sizeof (var));
      if (var[0] != 0) {
        
        //Comparación para la paginación en las alertas
        if (strncmp (var, "accion=prev", 11) == 0) {
          pag--; 
          if (pag < 1) pag = 1;
        }
        else if (strncmp (var, "accion=next", 11) == 0) {
          pag++;
          if (pag > 999) pag = 999;
        }
        else if (strncmp (var,"pagina=", 7) == 0){
          pag = atoi(var + 7);
          if (pag < 1) pag = 1;
        }
                 
        //Comparación para el modo de trabajo del sistema
        else if (strncmp(var, "modo=", 5) == 0) {
          modo_func = atoi(&var[5]);
        }
        
        //Comparación para poner hora de alarma
        else if (strncmp(var, "hora_dorm_per=", 14) == 0) {
          hora_dorm_per = atoi(var + 14);
        }
        else if (strncmp(var, "min_dorm_per=", 13) == 0) {
          min_dorm_per = atoi(var + 13);
          if(modo_func != 0x02)	osThreadFlagsSet(thweb_comTx, 0x01);
        }
        else if (strncmp(var, "hora_desp_per=", 14) == 0) {
          hora_desp_per = atoi(var + 14);
        }
        else if (strncmp(var, "min_desp_per=", 13) == 0) {
          min_desp_per = atoi(var + 13);
          if(modo_func != 0x02)	osThreadFlagsSet(thweb_comTx, 0x02);
        }
        
        //Comparación para gestión de tarjetas
        else if (strncmp(var, "id_tarj", 7) == 0) {
          int idx = atoi(&var[7]) - 1;
          char *val = strchr(var, '=');
          if (val && idx >= 0 && idx < 5) {
            strncpy(tabla_trabajadores[idx].id_tarjeta, val + 1, sizeof(tabla_trabajadores[idx].id_tarjeta) - 1);
          }
        }
        else if (strncmp(var, "taq_card", 8) == 0) {
          int idx = atoi(&var[8]) - 1;
          char *val = strchr(var, '=');
          if (val && idx >= 0 && idx < 5) {
            tabla_trabajadores[idx].taquilla = atoi(val + 1);
          }
        }
        else if (strncmp(var, "nom_trab", 8) == 0) {
          int idx = atoi(&var[8]) - 1;
          char *val = strchr(var, '=');
          if (val && idx >= 0 && idx < 5) {
            strncpy(tabla_trabajadores[idx].nombre_trabajador, val + 1, sizeof(tabla_trabajadores[idx].nombre_trabajador) - 1);
            if(idx == 4)  user_add(tabla_trabajadores);
          }
        }
        
        //Comparación para botones de alerta
        else if (strncmp(var, "accion=alarm_ok", 15) == 0) {
          // El usuario confirmó la Alarma Total
          modo_func = 0x01; //Se queda en modo activo
          alerta = 0;
          osThreadFlagsSet(thweb_comRx, 0x02);
        }
        else if (strncmp(var, "accion=alarm_false", 18) == 0) {
          // El usuario marcó Falsa Alarma
          modo_func = 0x02; //Se queda dormido
          alerta = 0;
          osThreadFlagsSet(thweb_comRx, 0x04);
        }
      }
    } while (data);
  }
}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  uint32_t len = 0U;
  uint8_t index = 0; //Variable para comprobar el número de alertas disponibles para mostrar
  
  if (*pcgi == 0 && env[0] == 'a' && actualizar_cache) {
    num_alertas = alarm_read_page(pag, buff_alertas);
    actualizar_cache = false;
  }
  
  switch (env[0]) {

    case 'a' : //escritura en página web de las alertas
      index = atoi(&env[2]) - 1;
      if (index < 15 && num_alertas > index) {
        len = sprintf(buf, &env[4], buff_alertas[index]);
      }else{
        len = sprintf(buf, &env[4], "---");
      }
      break;
      
    case 'c': //número de la página de alertas
      actualizar_cache = true;
      len = (uint32_t)sprintf (buf, &env[4], pag);
      break;
    
    case 'd': //escribir fecha y hora del sistema en la página web
      osMessageQueueGet(mid_Msg_Date, &fecha_rec, NULL, 0);
      switch(env[2]){
        case '1':
            len = (uint32_t)sprintf(buf, &env[4], fecha_rec.BufHour);
          break;
        case '2':
            len = (uint32_t)sprintf(buf, &env[4], fecha_rec.BufDate);
          break;
      }
      break;

    case 'e': //Checkbox que muestra estado de las taquillas
      switch(env[2]){
        case '1':
            len = (uint32_t)sprintf (buf, &env[4], (estado_taq & 0x01) ? "checked" : "");
          break;
        case '2':
            len = (uint32_t)sprintf (buf, &env[4], (estado_taq & 0x02) ? "checked" : "");
          break;
      }
      break;
      
    case 'f': //escribir peso de las taquillas en la página web
      switch(env[2]){
        case '1':
            sprintf(msg_web, "%.1f g",peso_taq1);
            len = (uint32_t)sprintf (buf, &env[4], msg_web);
          break;
        case '2':
            sprintf(msg_web, "%.1f g",peso_taq2);
            len = (uint32_t)sprintf (buf, &env[4], msg_web);
          break;
      }
      break;
      
    case 'b': //escribir intensidad del sistema en la página web
      switch(env[2]){
        case '1':
            sprintf(msg_web, "%.2f mA",intens);
            len = (uint32_t)sprintf (buf, &env[4], msg_web);
          break;
      }
      break;
      
    case 'j': //gestión de tarjetas
      if (strcmp(&env[2], "row") == 0) {
        uint32_t i = *pcgi; 
        if (carga_trabajadores){
					if (i < 5) {
						TRABAJADOR_t *t = &tabla_trabajadores[i];
						
						len = sprintf(buf, 
							"<tr>"
							"<td>%d</td>" //Número index
							"<td><input type='text' name='id_tarj%d' value='%s' disabled></td>" //ID Tarjeta
							"<td><select name='taq_card%d' disabled>"//Selección de la taquilla que abre
							"<option value='0' %s>Ninguna</option>"  //Estado "ninguna"
							"<option value='1' %s>Taquilla 1</option>"//Estado "taquilla 1"
							"<option value='2' %s>Taquilla 2</option>"//Estado "taquilla 2"
							"</select></td>"
							"<td><input type='text' name='nom_trab%d' value='%s' disabled></td>" //Nombre del empleado asignado
							"</tr>\r\n",          
							(i + 1),
							(i + 1), t->id_tarjeta,
							(i + 1), (t->taquilla == 0) ? "selected" : "", // Si val es 0, marca "ninguna"
							(t->taquilla == 1) ? "selected" : "", // Si val es 1, marca "taquilla 1"
							(t->taquilla == 2) ? "selected" : "", // Si val es 2, marca "taquilla 2"
							(i + 1), t->nombre_trabajador        // Para el nombre del empleado
						);
						
						i++;
						*pcgi = i;
						// El bit 31 de len indica que el servidor debe volver a llamar a 
						// esta función para la misma línea del script (repetir fila)
						len |= (1U << 31); 
					} else {
						// Ya no hay más filas, ponemos pcgi a 0 para la siguiente carga de página
						*pcgi = 0;
					}
				} else {
					user_read(tabla_trabajadores); //Carga los trabajadores desde la SD
					carga_trabajadores = 1;
				}
			}
    break;
      
    case 'k': // Indicador circular de estado del sistema
      switch(env[2]) {
        case '1':
          if (alerta == 0 && modo_func == 0x01) {
            len = (uint32_t)sprintf(buf, &env[4], "background-color: #00FF00;"); // Verde
          } else if (alerta == 0 && modo_func == 0x02){
            len = (uint32_t)sprintf(buf, &env[4], "background-color: #FFFF00;"); // Amarillo
          } else {
            len = (uint32_t)sprintf(buf, &env[4], "background-color: #FF0000;"); // Rojo
          }
          break;
          
        case '2':
          if (alerta == 0 && modo_func == 0x01) {
            len = (uint32_t)sprintf (buf, &env[4], "<span style='color:green;'>ACTIVO</span>");
          } else if (alerta == 0 && modo_func == 0x02){
            len = (uint32_t)sprintf (buf, &env[4], "<span style='color:yellow;'>STOP</span>");
          } else {
            len = (uint32_t)sprintf (buf, &env[4], "<span style='color:red;'>ALERTA</span>");
          }
          break;
      }
    break;
      
    case 'l': // Botones de gestión de alarma
      if (alerta == 1) { // Si hay alarma activa
        len = (uint32_t)sprintf(buf, 
        "<div style='margin-top:20px;'>"
        "<button type='submit' name='accion' value='alarm_ok' style='background-color:red; color:white; padding:10px;'>PASAR A ACTIVO</button> "
        "<button type='submit' name='accion' value='alarm_false' style='background-color:gray; color:white; padding:10px;'>PASAR A STOP</button>"
        "</div>");
      } else {
      // Si no hay alarma, devolvemos una cadena vacía
        len = (uint32_t)sprintf(buf, "");
      }
    break;
      
    case 'm': // Gestión de hora de dormir
      switch(env[2]) {
        case '1':
            len = (uint32_t)sprintf(buf, &env[4], hora_dorm_per);
          break;
        case '2':
            len = (uint32_t)sprintf(buf, &env[4], min_dorm_per);
          break;
        case '3':
            len = (uint32_t)sprintf(buf, &env[4], hora_desp_per);
          break;
        case '4':
            len = (uint32_t)sprintf(buf, &env[4], min_desp_per);
          break;
      }
    break;
      
    case 'n': // Gestión de nueva nfc
      len = sprintf(buf, "%02X%02X%02X%02X", id_nfc_new[3], id_nfc_new[2], id_nfc_new[1], id_nfc_new[0]);
    break;
     
  }
  return (len);
}

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic pop
#endif
