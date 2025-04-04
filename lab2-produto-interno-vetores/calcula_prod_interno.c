/**
 * Calcula o produto interno de dois vetores usando threads para cada produto i x i e a thread principal para o somatório completo
 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <math.h>

#define SIZE_FLOAT sizeof(float)
#define SIZE_LONG_INT sizeof(long int)
#define SIZE_DOUBLE sizeof(double)
#define SIZE_THREAD sizeof(pthread_t)

long int size; // vector size
float *vecOne; // vector one
float *vecTwo; // vector two
double originalInternalProduct; // internal product from file

typedef struct {
    long int start;
    long int end;
    double partialSum;
} ThreadArgs;

int generateError(int errorCode) {
    switch (errorCode) {
        case 1:
            fprintf(stderr, "\033[31mErro: malloc()\033[0m\n");
            break;
        case 2:
            fprintf(stderr, "\033[31mErro: Use ./<arquivo executavel> <arquivo de leitura> <numero threads>\033[0m\n");
            break;
        case 3:
            fprintf(stderr, "\033[31mErro: pthread_create()\033[0m\n");
            break;
        case 4:
            fprintf(stderr, "\033[31mErro: pthread_join()\033[0m\n");
            break;
        case 5:
            fprintf(stderr, "\033[31mErro: Carregamento do arquivo de entrada\033[0m\n");
            break;
        case 6:
            fprintf(stderr, "\033[31mErro: fopen()\033[0m\n");
            break;
        case 7:
            fprintf(stderr, "\033[31mErro: Leitura das dimensoes do arquivo de entrada\033[0m\n");
            break;
        case 8:
            fprintf(stderr, "\033[31mErro: Erro de leitura dos elementos do vetor\033[0m\n");
            break;
        default:
            fprintf(stderr, "\033[31mErro: Falha inesperada\033[0m\n");
            break;
    }

    return errorCode;
}

void *InternalProduct(void *arg) {
    ThreadArgs *args = (ThreadArgs *) arg;
    double sum = 0;

    for (long int i = args->start; i < args->end; i++) {
        sum += (double)vecOne[i] * (double)vecTwo[i];
    }

    args->partialSum = sum;
    pthread_exit(NULL);
}

int loadFileIntoVariables(const char *arg) {
    FILE *inputFile;
    size_t inputReturn;

    // read vectors file:
    inputFile = fopen(arg, "rb");
    if(inputFile == NULL) { return generateError(6); } 

    // vectors size loading:
    inputReturn = fread(&size, SIZE_LONG_INT, 1, inputFile);
    if(!inputReturn) { return generateError(7); }

    // vectors loading:
    vecOne = (float*) malloc (SIZE_FLOAT * size);
    vecTwo = (float*) malloc (SIZE_FLOAT * size);
    if(vecOne == NULL || vecTwo == NULL) { return generateError(1); }

    inputReturn = fread(vecOne, SIZE_FLOAT, size, inputFile);
    if(inputReturn < size) { return generateError(8); }
    
    inputReturn = fread(vecTwo, SIZE_FLOAT, size, inputFile);
    if(inputReturn < size) { return generateError(8); }

    // original internal product loading:
    inputReturn = fread(&originalInternalProduct, SIZE_DOUBLE, 1, inputFile);

    fclose(inputFile);
}

double calculateError(double originalSum, double threadsSum) {
    return fabs((originalSum - threadsSum) / originalSum) * 100;
}

int main(int argc, char *argv[]) {
    pthread_t *tid;
    int nThreads;
    double totalSum = 0;
    ThreadArgs *threadArgs = (ThreadArgs *) malloc(sizeof(ThreadArgs) * nThreads);
    if(threadArgs == NULL) { return generateError(1); }

    if(argc < 3) { return generateError(2); }

    int fileLoadStatus = loadFileIntoVariables(argv[1]);

    if (fileLoadStatus != 0) { return generateError(5); } 

    nThreads = atoi(argv[2]);
    tid = (pthread_t *) malloc(SIZE_THREAD * nThreads);
    if(tid == NULL)  { return generateError(1); }

    long int blockSize = size / nThreads;
    long int remaining = size % nThreads;

    long int start = 0;

    for (int i = 0; i < nThreads; i++) {
        threadArgs[i].start = start;
        threadArgs[i].end = start + blockSize + (i < remaining ? 1 : 0);
        threadArgs[i].partialSum = 0;
    
        if (pthread_create(&tid[i], NULL, InternalProduct, (void*) &threadArgs[i])) { return generateError(3); }

        start = threadArgs[i].end;
    }

    for(int i=0; i < nThreads; i++) {
        if (pthread_join(tid[i], NULL)) { return generateError(4); }
        totalSum += threadArgs[i].partialSum;
    }

    printf("Soma original (do arquivo): %.26lf\n", originalInternalProduct);

    printf("Soma com threads: %.26f\n", totalSum);

    double error = calculateError(originalInternalProduct, totalSum);
    printf("Erro percentual: %.26lf% %\n", error);

    free(vecOne);
    free(vecTwo);

    return 0;
}
