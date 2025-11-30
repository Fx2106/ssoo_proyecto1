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
char texto[MAX_TEXTO];                  
int texto_len = 0;                  

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
    printf("CACHE\n");
    for (int i = 0; i < NUM_FILAS; ++i) {
        /* muestra linea y etiqueta */
        printf("Linea %d  ETQ %02X  | ", i, tbl[i].ETQ);
        /* muetra los bytes */
        for (int b = TAM_LINEA - 1; b >= 0; --b) {
            printf("%02X ", tbl[i].Data[b]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(void) {
    /* cache inicial*/
    LimpiarCACHE(CACHE);

     /* eseña cache incial */
    VolcarCACHE(CACHE);
    
    return 0;
}