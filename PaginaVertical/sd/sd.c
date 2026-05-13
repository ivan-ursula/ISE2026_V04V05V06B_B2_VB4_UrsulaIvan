#include "stm32f4xx_hal.h"
#include "sd.h"
#include "stdio.h"
#include "string.h"
#include "rl_fs.h"

osThreadId_t th_id_SD;

osMessageQueueId_t mid_Msg_SD;
static int  Init_MsgQueue_SD (void);

static int sd_init(void);
void th_SD(void *argument);      

int Init_MsgQueue_SD (void) {
	
  mid_Msg_SD = osMessageQueueNew(MSGQUEUE_OBJECTS_SD, sizeof(MSGQUEUE_OBJ_SD), NULL);
	if (mid_Msg_SD == NULL) {
		return (-1) ;
	}
	
	return(0);
}
int Init_th_SD (void) 
{
	th_id_SD = osThreadNew(th_SD, NULL, NULL);
 
  return(0);
}
void th_SD (void *argument) 
{
	Init_MsgQueue_SD();
	sd_init();
	
  while (1)
	{
		osThreadYield();
  }
}
int sd_init(void) 
{
    fsStatus stat;

    stat = finit("M0:");
    if (stat != fsOK) return -1;

    stat = fmount("M0:");
    if (stat != fsOK) return -1;
	
    return 0;
}

int user_add(TRABAJADOR_t trabajador[])
{
    FILE *f;
    uint8_t i = 0;

    f = fopen("usuario.txt", "w");
    if (f == NULL) {
        return -1;
    }

    do{
      fprintf(f, "%d - %d - %s\n", 
              trabajador[i].id_tarjeta,
              trabajador[i].taquilla,
              trabajador[i].nombre_trabajador);
      i++;
    }while(i < 5);
    
    fclose(f);

    return 0;
}

int user_edit(const char *nfc_id_target, const char *new_user)
{
    FILE *fin, *ftmp;
    char line[128];
    int id;
    char nfc_id[64];
    char user[64];
    int found = 0;

    fin = fopen("usuario.txt", "r");
    if (fin == NULL) return -1;

    ftmp = fopen("temp.txt", "w");
    if (ftmp == NULL) {
        fclose(fin);
        return -2;
    }

    while (fgets(line, sizeof(line), fin) != NULL) {
        if (sscanf(line, "%d;%63[^;];%63[^\r\n]", &id, nfc_id, user) == 3) {
            if (strcmp(nfc_id, nfc_id_target) == 0) {
                fprintf(ftmp, "%d;%s;%s\n", id, nfc_id, new_user);
                found = 1;
            } else {
                fprintf(ftmp, "%d;%s;%s\n", id, nfc_id, user);
            }
        }
    }

    fclose(fin);
    fclose(ftmp);

    if (!found) {
        fdelete("temp.txt", NULL);
        return -3;
    }

    if (fdelete("usuario.txt", NULL) != fsOK) return -4;
    if (frename("temp.txt", "usuario.txt") != fsOK) return -5;

    return 0;
}

int user_delete(const char *nfc_id_target)
{
    FILE *fin, *ftmp;
    char line[128];
    int id;
    char nfc_id[64];
    char user[64];
    int found = 0;

    fin = fopen("usuario.txt", "r");
    if (fin == NULL) return -1;

    ftmp = fopen("temp.txt", "w");
    if (ftmp == NULL) {
        fclose(fin);
        return -2;
    }

    while (fgets(line, sizeof(line), fin) != NULL) {
        if (sscanf(line, "%d;%63[^;];%63[^\r\n]", &id, nfc_id, user) == 3) {
            if (strcmp(nfc_id, nfc_id_target) == 0) {
                found = 1;   // no se copia => eliminado
            } else {
                fprintf(ftmp, "%d;%s;%s\n", id, nfc_id, user);
            }
        }
    }

    fclose(fin);
    fclose(ftmp);

    if (!found) {
        fdelete("temp.txt", NULL);
        return -3;
    }

    if (fdelete("usuario.txt", NULL) != fsOK) return -4;
    if (frename("temp.txt", "usuario.txt") != fsOK) return -5;

    return 0;
}

void alarm_write(char *date, char *time, char *texto)
{
  FILE *f;
  
  f = fopen("alarma.txt", "a");
  
  if (f != NULL) {
    fprintf(f, "%s - ", date);
    fprintf(f, "%s - ", time);
    fprintf(f, "%s\n", texto);
    fclose(f);
  }
}

int alarm_read_page(uint16_t pag_actual, char buffer_dest[10][90]) {
    FILE *f;
    char line[128];
    int total_lineas = 0;
    int line_idx = 0;
    int alertas_leidas = 0;
		int start_line = 0;
	  int end_line = 0;

    // --- PASO 1: LIMPIEZA TOTAL ---
    // Esto evita que si la página anterior tenía 10 alertas 
    // y esta solo tiene 3, las otras 7 se queden con texto viejo.
    for (int i = 0; i < 10; i++) {
        strcpy(buffer_dest[i], ""); // O "---" si prefieres visualmente
    }

    f = fopen("alarma.txt", "r");
    if (f == NULL) return 0;

    // --- PASO 2: CONTEO ---
    while (fgets(line, sizeof(line), f) != NULL) {
        total_lineas++;
    }

    if (total_lineas == 0) { fclose(f); return 0; }

    // --- PASO 3: CÁLCULO DE LÍMITES ---
    start_line = total_lineas - ((pag_actual - 1) * 10);
    end_line = start_line - 10;
    if (end_line < 0) end_line = 0; // Evita índices negativos

    // --- PASO 4: LECTURA Y POSICIONAMIENTO ---
    rewind(f);
    line_idx = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        line_idx++;
        if (line_idx <= start_line && line_idx > end_line) {
            // La posición 0 será la más cercana al final del archivo (más nueva)
            int pos_buffer = start_line - line_idx; 
            
            if (pos_buffer >= 0 && pos_buffer < 10) {
                line[strcspn(line, "\r\n")] = 0;
                strncpy(buffer_dest[pos_buffer], line, 89);
                buffer_dest[pos_buffer][89] = '\0';
                alertas_leidas++;
            }
        }
    }
    fclose(f);
    return alertas_leidas;
}
