#include <stdio.h>
#include <unistd.h>

#define TAM_LINEA 16
#define NUM_FILAS 8

#define TAM_BITS_BLOQUE 8
#define TAM_BITS_ETQ 5
#define TAM_BITS_LINEA 3
#define TAM_BITS_PALABRA 4

typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void VolcarCACHE(T_CACHE_LINE *tbl);
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);

int ExtraerBits(int addr, int posicion, int numBits);

int globaltime = 0;
int numfallos = 0;
char texto[100];

int main(int argc, char *argv){
    T_CACHE_LINE cache[NUM_FILAS];
    LimpiarCACHE(cache);

    unsigned char Simul_RAM[4096];
    FILE *contenidoRAM;
    contenidoRAM = fopen("CONTENTS_RAM.bin", "rb"); // Lectura y tratamiento de errores del archivo de CONTENTS_RAM.bin
    if(!contenidoRAM){
        printf("ERROR: No existe el archivo");
        return -1;
    }
    fread(&Simul_RAM,sizeof(Simul_RAM),sizeof(unsigned char),contenidoRAM);
    fclose(contenidoRAM);

    FILE *memorias;
    memorias = fopen("accesos_memoria.txt", "r"); // Lectura y tratamiento de errores del archivo de accesos_memoria.txt
    if(!memorias){
        printf("ERROR: No existe el archivo");
        return -1;
    }

    unsigned int addr;
    int accesosTotales = 0;
    while (fscanf(memorias, "%X", &addr) != EOF) { // Bucle que se encarga de leer todas las direcciones de memoria y tratarlas
        globaltime += 1;
        int ETQ, palabra, linea, bloque;
        ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);
        if(cache[linea].ETQ != ETQ){ // Comprobante de si hay correspondencia con la cache
            numfallos++;
            printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n", globaltime, numfallos, addr, ETQ, linea, palabra, bloque);
            TratarFallo(cache, Simul_RAM, ETQ, linea, bloque);
            globaltime += 10;
        }
        printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n", globaltime, addr, ETQ, linea, palabra, bloque);
        texto[accesosTotales] = (char)cache[linea].Data[palabra];
        VolcarCACHE(cache);
        sleep(1);
        accesosTotales++;
    }
    fclose(memorias);
    
    printf("Accesos totales: %d; fallos: %d; Tiempo medio: %.2f\n", accesosTotales, numfallos, (float)globaltime/(float)accesosTotales);
    printf("Texto leído: %s\n", texto);

    FILE *CONTENTS_CACHE;
    CONTENTS_CACHE = fopen("CONTENTS_CACHE.bin", "wb"); // Creación del archivo CONTENTS_CACHE.bin que vuelva todos los valores de la cache
    for (int i = 0; i < NUM_FILAS; i++){
        for (int j = 0; j < TAM_LINEA; j++){
            fwrite(&cache[i].Data[j], 1, 1, CONTENTS_CACHE);
        }
    }
    fclose(CONTENTS_CACHE);
    return 0;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){ // Cuando encuentra un fallo asigna a la linea de cache correspondiente la etiqueta y los valores de los datos
    printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);
    tbl[linea].ETQ = ETQ;
    for (int i = 0; i < TAM_LINEA; i++){
        tbl[linea].Data[i] = MRAM[bloque*TAM_LINEA+i];
    }
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){ // Con la función extraer bits sacamos los respectivos bits de cada campo de una dirección
    *ETQ = ExtraerBits(addr, 8, TAM_BITS_ETQ);
    *linea = ExtraerBits(addr, 5, TAM_BITS_LINEA);
    *palabra = ExtraerBits(addr, 1, TAM_BITS_PALABRA);
    *bloque = ExtraerBits(addr, 5, TAM_BITS_BLOQUE);
}

void VolcarCACHE(T_CACHE_LINE *tbl){ // Función que se encarga de mostrar por consola la cache
    for (int i = 0; i < NUM_FILAS; i++){
        printf("ETQ:%X\tData", tbl[i].ETQ);
        for (int j = TAM_LINEA-1; j >= 0 ; j--){
            printf(" %X", tbl[i].Data[j]);
        }
        printf("\n");
    }
    printf("\n");
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){ // Asigna los valores por defecto a la cache etiqueta 0xFF y los datos a 0x23
    for (int i = 0; i < NUM_FILAS; i++){
        tbl[i].ETQ = 0xFF;
        for (int j = 0; j < TAM_LINEA; j++){
            tbl[i].Data[j] = 0x23;
        }
    }
}

int ExtraerBits(int addr, int posicion, int numBits){
    return (((1 << numBits) - 1) & (addr >> (posicion - 1))); // Creamos una mascara con el numero de bits que queremos sacar de la dirección y luego desplazamos la dirección el numero que posiciones desde donde queremos empezar y despues comparamos la mascara con el addres y nos devuelve los bits que queremos
}