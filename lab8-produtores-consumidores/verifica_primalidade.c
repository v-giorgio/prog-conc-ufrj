#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <unistd.h>

#define MAX_CONSUMIDORES 1000
#define MAX_BUFFER 1000

long long int *buffer; // buffer compartilhado
int N, M, C; // N: quantidade de números a gerar, M: tamanho do buffer, C: quantidade de consumidores
int in = 0, out = 0; // indices de entrada e saída no buffer
int numeros_primos = 0;
int finalizado = 0;
int *primos_por_thread; // contador de primos por thread para verificar a vencedora
int total_consumido = 0; // contador de números consumidos (foi adicionado para evitar que consumidores finalizem antes do produtor)

sem_t mutex; // semáforo para controle de acesso ao buffer
sem_t cheio; // semáforo para indicar que há itens no buffer
sem_t vazio; // semáforo para indicar que há espaço no buffer

int ehPrimo(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void* produtor(void* arg) {
    for (int i = 0; i < N; i++) { // encher o buffer com N números
        sem_wait(&vazio); // espera até que haja espaço no buffer
        sem_wait(&mutex); // entra na seção crítica (acesso ao buffer)
        buffer[in] = i;
        in = (in + 1) % M;
        sem_post(&mutex); // libera da seção crítica
        sem_post(&cheio); // sinaliza que buffer recebeu um novo item
    }

    sem_wait(&mutex);
    finalizado = 1; // var que sinaliza que o produtor terminou de produzir
    sem_post(&mutex);

    // libera todos os consumidores que possam estar bloqueados
    for (int i = 0; i < C; i++) sem_post(&cheio);

    pthread_exit(NULL);
}

void* consumidor(void* arg) {
    int id = *((int*) arg);
    free(arg);
    while (1) {
        sem_wait(&cheio); // espera até que haja pelo menos um item no buffer (cheio > 0)
        sem_wait(&mutex);

        if (finalizado && total_consumido >= N) {
            sem_post(&mutex); // evitar deadlock
            break;
        }

        long long int numero = buffer[out];
        out = (out + 1) % M;
        total_consumido++;

        sem_post(&mutex);
        sem_post(&vazio); // sinaliza que o buffer agora tem um espaço vazio a mais (vazio++); libera produtor se estiver bloqueado

        if (ehPrimo(numero)) {
            sem_wait(&mutex); // entra na seção crítica para atualizar o contador de primos (var compartilhada)
            primos_por_thread[id]++;
            numeros_primos++;
            sem_post(&mutex);
        }
    }
    pthread_exit(NULL);
}

int main() {
    printf("Digite N (quantidade de numeros a gerar): ");
    scanf("%d", &N);
    printf("Digite M (tamanho do buffer): ");
    scanf("%d", &M);
    printf("Digite C (quantidade de consumidores): ");
    scanf("%d", &C);

    if (C > MAX_CONSUMIDORES || M > MAX_BUFFER) {
        // limitar quantidade de consumidores e tamanho do buffer
        printf("Limites excedidos: MAX_CONSUMIDORES=%d, MAX_BUFFER=%d\n", MAX_CONSUMIDORES, MAX_BUFFER);
        return 1;
    }

    buffer = malloc(sizeof(long long int) * M);
    primos_por_thread = calloc(C, sizeof(int)); // já inicializar a qtd de primos por thread com 0
    pthread_t prod, cons[C]; 

    sem_init(&mutex, 0, 1); // semáforo como mutex binário, inicializado como 1 (disponível), para controle de acesso ao buffer tanto dos prod quanto cons
    sem_init(&vazio, 0, M); // semáforo para indicar que o buffer está vazio, inicializado com M (tamanho do buffer) - prod aguarda até que haja espaço
    sem_init(&cheio, 0, 0); // semáforo para indicar que o buffer está cheio, inicializado com 0 - cons aguarda até que haja itens no buffer

    pthread_create(&prod, NULL, produtor, NULL); // cria 1 produtor
    for (int i = 0; i < C; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&cons[i], NULL, consumidor, id); // cria C consumidores
    }

    pthread_join(prod, NULL);
    for (int i = 0; i < C; i++) {
        pthread_join(cons[i], NULL);
    }

    // Resultado final
    int vencedora = 0;
    for (int i = 1; i < C; i++) {
        if (primos_por_thread[i] > primos_por_thread[vencedora])
            vencedora = i;
    }

    printf("\nTotal de primos encontrados: %d\n", numeros_primos);
    printf("Thread vencedora: Consumidor %d com %d primos\n",
           vencedora, primos_por_thread[vencedora]);

    // liberar recursos
    free(buffer);
    free(primos_por_thread);
    sem_destroy(&mutex);
    sem_destroy(&cheio);
    sem_destroy(&vazio);

    return 0;
}
