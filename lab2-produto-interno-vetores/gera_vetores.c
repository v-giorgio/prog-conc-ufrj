/**
 * Gera dois vetores de float com dimensão N e calcula o produto interno deles sequencialmente
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 100

#define SIZE_FLOAT sizeof(float)
#define SIZE_LONG_INT sizeof(long int)
#define SIZE_DOUBLE sizeof(double)

int generateError(int errorCode) {
    switch (errorCode) {
        case 1:
            fprintf(stderr, "\033[31mErro: Digite: ./<nome arquivo exec> <dimensao> <nome arquivo saida>\033[0m\n");
            break;
        case 2:
            fprintf(stderr, "\033[31mErro: Alocacao da memoria do vetor\033[0m\n");
            break;
        case 3:
            fprintf(stderr, "\033[31mErro: Abertura do arquivo\033[0m\n");
            break;
        case 4:
            fprintf(stderr, "\033[31mErro: Escrita no arquivo\033[0m\n");
            break;
        default:
            fprintf(stderr, "\033[31mErro: Falha inesperada\033[0m\n");
            break;
    }

    return errorCode;
}

void fillVector(float *vector, long int size, unsigned int seedOffset) {
    int randomNumber;
    float randomElement;
    
    srand(time(NULL) + seedOffset);
    for (long int i = 0; i < size; i++) {
        randomNumber = rand();
        randomElement = ((randomNumber % MAX_VALUE) / ((randomNumber & 1) ? 2.0 : 3.0)) * ((randomNumber & 1) ? -1 : 1);
        vector[i] = randomElement;

        printf("Vetor[%ld] = %.2f\n", i, vector[i]);
    }
}

double calculateInternalProduct(const float *vec1, const float *vec2, long int size) {
    double result = 0.0;
    for (long int i = 0; i < size; i++) {
        result += vec1[i] * vec2[i];
    }

    return result;
}

int writeFile(const char *fileName, float *vectorOne, float *vectorTwo, long int size, double internalProduct) {
    FILE * fileOutput;
    size_t fileReturn;

    fileOutput = fopen(fileName, "wb");
    if (!fileOutput) { return generateError(3); }

    fileReturn = fwrite(&size, SIZE_LONG_INT, 1, fileOutput);
    fileReturn = fwrite(vectorOne, SIZE_FLOAT, size, fileOutput);
    if (fileReturn < size) { return generateError(4); }

    fileReturn = fwrite(vectorTwo, SIZE_FLOAT, size, fileOutput);

    fileReturn = fwrite(&internalProduct, SIZE_DOUBLE, 1, fileOutput);

    fclose(fileOutput);

    return 0;
}

int main(int argc, char *argv[]) {
    long int size;
    int exitStatus;

    if (argc < 3) { return generateError(1); }
    size = atoi(argv[1]);

    float *vectorOne = (float*) malloc(SIZE_FLOAT * size);
    float *vectorTwo = (float*) malloc(SIZE_FLOAT * size);
    if (!vectorOne || !vectorTwo) { return generateError(2); }
        
    printf("Gerando vetor 1\n");
    fillVector(vectorOne, size, 0);

    printf("Gerando vetor 2\n");
    fillVector(vectorTwo, size, 1);

    double internalProduct = calculateInternalProduct(vectorOne, vectorTwo, size);

    printf("Produto interno dos vetores 1 e 2: %.26lf\n", internalProduct);

    exitStatus = writeFile(argv[2], vectorOne, vectorTwo, size, internalProduct);
    
    free(vectorOne);
    free(vectorTwo);

    return exitStatus;
}
