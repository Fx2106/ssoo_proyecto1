#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define NUM_FILAS 8
#define TAM_LINEA 16
#define SIMUL_RAM_SIZE 4096
#define MAX_TEXTO 100

typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

T_CACHE_LINE CACHE[NUM_FILAS];
unsigned char Simul_RAM[SIMUL_RAM_SIZE];
int globaltime = 0;
int numfallos = 0;
char texto[MAX_TEXTO];
int texto_len = 0;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ, int linea, int bloque);