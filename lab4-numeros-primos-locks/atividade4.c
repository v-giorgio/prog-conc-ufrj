/* [Prog. Conc.] Lab. 4 - Atividade 4 */
/* Contagem de números primos usando threads e variável compartilhada com locks, evitando violação de atomicidade */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

long long int N; // número de elementos
int nthreads;    // número de threads
long long int atual = 1; // número atual sendo testado
long long int nprimos = 0; // quantidade total de primos encontrados

pthread_mutex_t mutex; // proteger as variáveis compartilhadas

// fn para verificar primalidade (dada no enunciado)
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0; // 0 e 1 não são primos
    if (n == 2) return 1; // 2 é primo
    if (n % 2 == 0) return 0; // múltiplos de 2 não são primos
    for (i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return 0; // múltiplos de i não são primos
    return 1; // é primo
}

// fn executada pelas threads
void *ExecutaTarefa(void *arg) {
    long long int num, inicio, fim;
    int tamanho_bloco = 100; // tamanho do bloco de números a serem verificados (otimização após primeiros testes)
    long long int meu_nprimos = 0; // contador local de primos encontrados (otimização, reduzindo o número de locks usados)
    
    while (1) {
        pthread_mutex_lock(&mutex);
        if (atual > N) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // essa lógica abaixo foi otimizada para processar blocos de números ao invés de um por um (era pouco trabalho para cada thread - muito overhead de troca de contexto)
        inicio = atual;
        fim = atual + tamanho_bloco - 1;
        if (fim > N) fim = N; // garante que não ultrapasse N
        atual = fim + 1;
        pthread_mutex_unlock(&mutex);
    
        // agora fora do lock, processa o bloco de números
        for (num = inicio; num <= fim; num++) {
            if (ehPrimo(num)) {
                meu_nprimos++;
            }
        }
    }

    pthread_mutex_lock(&mutex);
    nprimos += meu_nprimos;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t *tid; // ponteiro para as threads
    struct timeval inicio, fim; // variáveis para medir o tempo 
    double delta; // tempo de execução
    FILE *fsaida; // arquivo de saída para os resultados

    // verificação de argumentos
    if (argc < 3) {
        printf("Digite: %s <N> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]); // número máximo a ser verificado (parse para long long int)
    nthreads = atoi(argv[2]); // número de threads (parse para int)

    // alocar das threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) { puts("ERRO--malloc"); return 2; }

    // inicializa o mutex
    pthread_mutex_init(&mutex, NULL);

    // marca tempo inicial
    gettimeofday(&inicio, NULL);

    // cria as threads
    for (long int t = 0; t < nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, NULL)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    // aguarda todas as threads
    for (int t = 0; t < nthreads; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }

    // marca tempo final
    gettimeofday(&fim, NULL);

    // finaliza o mutex
    pthread_mutex_destroy(&mutex);

    // calcula tempo de execução em segundos
    delta = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec)/1000000.0;

    // Exibe resultados
    printf("Total de primos entre 1 e %lld: %lld\n", N, nprimos);
    printf("Tempo de execucao: %lf segundos\n", delta);

    // vamos supor que o tempo sequencial é igual ao tempo medido com 1 thread.
    double tempo_seq = delta; // se você rodar com 1 thread, tempo_seq será delta
    double aceleracao = 1.0;  // com 1 thread, aceleracao é 1
    double eficiencia = 1.0;  // com 1 thread, eficiencia é 1

    if (nthreads > 1) {
        // carregar o tempo sequencial do arquivo de 1 thread (gerado numa primeira execução com 1 thread)
        char filename_seq[100];
        sprintf(filename_seq, "resultados_N%lld_1threads.txt", N); // arquivo de 1 thread
        FILE *fseq = fopen(filename_seq, "r");

        if (fseq != NULL) {
            fscanf(fseq, "%lf", &tempo_seq);
            fclose(fseq);
            aceleracao = tempo_seq / delta;
            eficiencia = aceleracao / nthreads;
        } else {
            printf("Arquivo 'tempo_sequencial.txt' nao encontrado. Assumindo execucao sequencial.\n");
        }
    }

    char filename[100];
    sprintf(filename, "resultados_N%lld_%dthreads.txt", N, nthreads); // um arquivo por N e número de threads

    // cria o arquivo de saída
    fsaida = fopen(filename, "w");
    if (fsaida == NULL) {
        printf("ERRO ao criar arquivo resultados.txt\n");
        return 3;
    }

    fprintf(fsaida, "%lf\n", delta);       // linha 1: tempo de execução
    fprintf(fsaida, "%lf\n", aceleracao);   // linha 2: aceleração
    fprintf(fsaida, "%lf\n", eficiencia);   // linha 3: eficiência

    fclose(fsaida);

    printf("Resultados gravados no arquivo 'resultados.txt'.\n");

    free(tid);
    return 0;
}
