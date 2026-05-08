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
int user_add(int id, const char *nfc_id, const char *user);
int user_edit(const char *nfc_id_target, const char *new_user);
int user_delete(const char *nfc_id_target);

void events_sd(void);

void sd_test(void);

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
	sd_test();
	
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

int user_add(int id, const char *nfc_id, const char *user)
{
    FILE *f;

    f = fopen("usuario.txt", "a");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "%d;%s;%s\n", id, nfc_id, user);
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

void sd_test(void)
{
    FILE *f;
    char line[128];

    // 1. Crear fichero inicial
    f = fopen("usuario.txt", "w");
    if (f != NULL) {
        fprintf(f, "1;AAAA1111;Ivan\n");
        fprintf(f, "2;BBBB2222;Samuel\n");
        fprintf(f, "3;CCCC3333;Lucia\n");
        fclose(f);
    }

    // 2. Editar
    user_edit("BBBB2222", "Carlos");

    // 3. Añadir
    user_add(4, "DDDD4444", "Marta");

    // 4. Eliminar
    user_delete("AAAA1111");

    // 5. Leer para verificar
    f = fopen("usuario.txt", "r");
    if (f != NULL) {
        while (fgets(line, sizeof(line), f) != NULL) {
            // aquí puedes mandar la línea por UART, LCD o debugger
        }
        fclose(f);
    }
}