#include "stm32f4xx_hal.h"
#include "sd.h"
#include "stdio.h"
#include "string.h"
#include "rl_fs.h"

/*Variables*/
char id_nfc_new [9]; //Registra última tarjeta introducida

/*----------------------------------------------------------------------------
*      Inicialización de la SD
 *---------------------------------------------------------------------------*/
int sd_init(void){
    fsStatus stat;
    stat = finit("M0:");
    if (stat != fsOK) return -1;
    stat = fmount("M0:");
    if (stat != fsOK) return -1;
    return 0;
}


/*----------------------------------------------------------------------------
*      Funciones de la SD
 *---------------------------------------------------------------------------*/
 /*Añade la información de las tarjetas NFC a la SD*/
int user_add(TRABAJADOR_t trabajador[]){
    FILE *f;
    uint8_t i = 0;

    f = fopen("usuario.txt", "w");
    if (f == NULL) return -1;

    do{fprintf(f, "%s - %d - %s\n", 
       trabajador[i].id_tarjeta,
       trabajador[i].taquilla,
       trabajador[i].nombre_trabajador
      );
      i++;
    }while(i < 5);
		
    fclose(f);
    return 0;
}

/*Lee la información en la SD sobre las tarjetas NFC */
int user_read(TRABAJADOR_t trabajador[]){
	FILE *f;
	int i = 0;
	char line[128];
	int taquilla_temp = 0;

	f = fopen("usuario.txt", "r");
	if (f == NULL) return -1;

	while (fgets(line, sizeof(line), f) != NULL && i < 5){
			// Buscamos el ID en el archivo (que está escrito como texto hex)
			if (sscanf(line, "%8s - %d - %s\n",
					trabajador[i].id_tarjeta,
					&taquilla_temp,
					trabajador[i].nombre_trabajador) >= 2){
					trabajador[i].taquilla = (uint8_t)taquilla_temp;
					i++;
			}
	}
	
	fclose(f);
	return 0;
}

 /*Busca en si la NFC introducida se encuentra ya registrada*/
int nfc_search(const char *nfc_id_target){
    FILE *f;
    char line[128];
    unsigned int id_hex_file;
    unsigned int id_target_numeric;
    char user[64];
    int locker_num;
    int found = 0;

    // 1. CONVERSIÓN ROBUSTA
    // Usamos unsigned char para evitar problemas de signo con valores altos como 0xAE
    const uint8_t *b = (const uint8_t *)nfc_id_target;

    // INTENTO A: Orden normal (Big Endian) -> 0x08AE3A5D
    // id_target_numeric = ((unsigned int)b[0] << 24) | ((unsigned int)b[1] << 16) | ((unsigned int)b[2] << 8) | (unsigned int)b[3];

    // INTENTO B: Orden inverso (Little Endian) -> Muy común en lectores NFC
    // Si el numeric te daba 08AE0000, es probable que los datos útiles estén al principio
    // Probemos esta estructura que es la más estándar para IDs de 4 bytes:
    id_target_numeric = (unsigned int)b[0] | 
                        ((unsigned int)b[1] << 8) | 
                        ((unsigned int)b[2] << 16) | 
                        ((unsigned int)b[3] << 24);
												
    f = fopen("usuario.txt", "r");
    if (f == NULL) return -1;

    while (fgets(line, sizeof(line), f) != NULL) {
        // Buscamos el ID en el archivo (que está escrito como texto hex)
        if (sscanf(line, "%x - %d - %63s", &id_hex_file, &locker_num, user) >= 2) {
            if (id_hex_file == id_target_numeric) {
                found = locker_num;
                break;
            }
        }
    }
    
    fclose(f);
    return found;
}


 /*Escritura de una nueva alarma en la SD*/
void alarm_write(char *date, char *time, char *texto){
  FILE *f;

  f = fopen("alarma.txt", "a");
  
  if (f != NULL) {
    fprintf(f, "%s - ", date);
    fprintf(f, "%s - ", time);
    fprintf(f, "%s\n", texto);
    fclose(f);
  }
}

 /*Lectura de las alarmas del sistema. Se realiza de más nuevas a más viejas.*/
int alarm_read_page(uint16_t pag_actual, char buffer_dest[10][90]){
    FILE *f;
    char line[128];
    int total_lineas = 0;
    int line_idx = 0;
    int alertas_leidas = 0;
		int start_line = 0;
	  int end_line = 0;

    // Limpieza del buffer de destino
    for (int i = 0; i < 10; i++) {
        strcpy(buffer_dest[i], "");
    }

    f = fopen("alarma.txt", "r");
    if (f == NULL) return 0;

    //Cuenta del número de alertas
    while (fgets(line, sizeof(line), f) != NULL){
        total_lineas++;
    }

    if (total_lineas == 0) { fclose(f); return 0; }

    //Cálculo para la linea a leer
    start_line = total_lineas - ((pag_actual - 1) * 10);
    end_line = start_line - 10;
    if (end_line < 0) end_line = 0; // Evita índices negativos

    //Lectura de las alertas
    rewind(f);
    line_idx = 0;
    while (fgets(line, sizeof(line), f) != NULL){
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
