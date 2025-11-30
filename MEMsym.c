#include <stdio.h> 
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>     /*para usar sleep*/
#include <stdint.h>

/*CACHE*/
#define NUM_FILAS 8            
#define TAM_LINEA 16            
#define SIMUL_RAM_SIZE 4096 
#define MAX_TEXTO 100           

typedef struct {
    unsigned char ETQ;         
    unsigned char Data[TAM_LINEA]; 
} T_CACHE_LINE;

/*VARIABLES GLOBALES*/
T_CACHE_LINE CACHE[NUM_FILAS];          
unsigned char Simul_RAM[SIMUL_RAM_SIZE];      /*Memoria Principal*/
int globaltime = 0;                   
int numfallos = 0;                      
int num_accesos = 0;
char texto[MAX_TEXTO];
int texto_len=0;

/*PROTOTIPOS*/
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ, int linea, int bloque);



/* inicializa la caché: pone ETQ a 0xFF y Data a 0x23 en todas las líneas. */
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) {
    for (int i = 0; i < NUM_FILAS; ++i) {
        tbl[i].ETQ = 0xFF; /* line vacia */
        for (int j = 0; j < TAM_LINEA; ++j) {
            tbl[i].Data[j] = 0x23; /* valor incial enunciado */
        }
    }
}

/* muestra por pantalla el contenido actual de la caché en el formato requerido. */
void VolcarCACHE(T_CACHE_LINE *tbl) {
    printf("\n------ CONTENIDO CACHE ------\n");
    for (int i = 0; i < NUM_FILAS; ++i) {
        /* muestra linea y etiqueta */
        printf("Linea %d  ETQ %02X  | ", i, tbl[i].ETQ);
        /* muetra los bytes */
        for (int b = TAM_LINEA - 1; b >= 0; --b) {
            printf("%02X ", tbl[i].Data[b]);
        }
        printf("\n");
    }
    printf("-----------------------------\n\n");
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque) {
    /*usar solo 12 bits*/
    addr &= 0xFFF; 
    /* extraer la palabra */
    *palabra = addr & 0xF;       /* utilizamos mascara 0xF*/
    /* extraer la linea */
    *linea   = (addr >> 4) & 0x7;       /* desplazar 4 bits para aplicar la mascara 0x7  */
    /* extraer la etiqueta */
    *ETQ     = (addr >> 7) & 0x1F;       /* desplazar 7 bits para aplicar la mascara 0x1F */
    /* 4calcular numero de bloque */
    *bloque  = addr >> 4;                  
}


void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ, int linea, int bloque) {
    
	unsigned int addr_inicio = bloque * TAM_LINEA;
	
	numfallos++;
	globaltime+=20;
	
	memcpy(tbl[linea].Data, &MRAM[addr_inicio],TAM_LINEA);
	tbl[linea].ETQ = ETQ;
	
	printf("T: %d, Fallo de CACHE, Cargando bloque %02X en linea %02X\n", globaltime, bloque, linea);
}

int main(void) {
	/* Limpiamos cache */
	LimpiarCACHE(CACHE);
	
	/*Cargamos RAM*/
	FILE *ram=fopen("CONTENTS_RAM.bin", "rb");
	if (!ram){
		printf("Error: no se pudo abrir CONTENTS_RAM.bin\n");
		return -1;
	}

	size_t leidos = fread(Simul_RAM, 1, SIMUL_RAM_SIZE, ram);
	fclose(ram);

	if (leidos != SIMUL_RAM_SIZE){
		printf("Error: tamaño incorrecto de CONTENTS_RAM.bin (%zu bytes)\n", leidos);
		return -1;
	}

	/*Mostrar cache inicial*/
	VolcarCACHE(CACHE);

	FILE *f=fopen("accesos_memoria.txt", "r");
	if(!f){
		printf("Error: no se pudo abrir accesos_memoria.txt\n");
		return -1;
	}

	char linea_hex[20];
	unsigned int addr;
	
	/*Leer cada acceso*/
	while (fgets(linea_hex, sizeof(linea_hex),f)){
		addr = (unsigned int) strtol(linea_hex, NULL, 16);
		num_accesos++;

		int ETQ, palabra, linea, bloque;

		ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);

		if (CACHE[linea].ETQ == ETQ){
			globaltime +=1;
			unsigned char dato = CACHE[linea].Data[palabra];
			printf("T: %d, ACIERTO de cache, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",
			 globaltime, addr, ETQ, linea, palabra, dato);
			if(texto_len < MAX_TEXTO){
				texto[texto_len++]=dato;
			}
		}else{
			TratarFallo(CACHE, Simul_RAM, ETQ, linea, bloque);
			unsigned char dato = CACHE[linea].Data[palabra];
			if(texto_len < MAX_TEXTO){
				texto[texto_len++]=dato;
			}
			printf("T: %d, ACIERTO de CACHE (después de fallo), ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",
                  	 globaltime, addr, ETQ, linea, palabra, dato); 
		}
	
		sleep(1);

	}
	
	fclose(f);

	/*Volcar cache final*/
	VolcarCACHE(CACHE);

	/* Guardar la caché en binario */
	FILE *cache_out = fopen("CONTENTS_CACHE.bin", "wb"); 
	for(int i = 0; i < NUM_FILAS; i++)
        fwrite(CACHE[i].Data, 1, TAM_LINEA, cache_out);
	fclose(cache_out);

	printf("Accesos totales: %d; Fallos: %d; Tiempo medio: %.2f\n", 
        num_accesos, numfallos, (float)globaltime / num_accesos);
	
	texto[texto_len]='\0';
	printf("Texto leido: %s\n", texto);
	return 0;
} 

