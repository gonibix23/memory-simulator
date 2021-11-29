#include <stdio.h>

#define TAM_LINEA 16
#define NUM_FILAS 8

typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void IniciarCache(T_CACHE_LINE *cache);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void VolcarCACHE(T_CACHE_LINE *tbl);
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);

int main(int argc, char *argv){
    int globaltime = 0;
    int numfallos = 0;
    
    T_CACHE_LINE cache[NUM_FILAS];
    IniciarCache(cache);

    unsigned char Simul_RAM[4096];
    FILE *contenidoRAM;
    contenidoRAM = fopen("CONTENTS_RAM.bin", "rb");
    if(!contenidoRAM){
        printf("ERROR: No existe el archivo");
        return -1;
    }
    fread(&Simul_RAM,sizeof(Simul_RAM),1,contenidoRAM);
    fclose(contenidoRAM);

    FILE *memorias;
    memorias = fopen("accesos_memoria.txt", "r");
    if(!memorias){
        printf("ERROR: No existe el archivo");
        return -1;
    }
    fclose(memorias);
    
    VolcarCACHE(cache);
}

void IniciarCache(T_CACHE_LINE *cache){
    for (int i = 0; i < NUM_FILAS; i++){
        cache->ETQ = 0xFF;
        for (int j = 0; j < TAM_LINEA; j++){
            cache->Data[j] = 0x23;
        }
    }
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){

}

void VolcarCACHE(T_CACHE_LINE *tbl){
    for (int i = 0; i < NUM_FILAS; i++){
        printf("ETQ:%x\tData", tbl->ETQ);
        for (int j = 0; j < TAM_LINEA; j++){
            printf(" %x", tbl->Data[j]);
        }
        printf("\n");
    }
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){

}