# Identificação de números primos com produtor/consumidor e semáforos

Esta atividade tem como objetivo mostrar, de forma prática, como utilizar o padrão **produtor/consumidor** com **semáforos** e **variáveis compartilhadas** para avaliar **primalidade** em C. A sincronização correta entre threads é garantida por semáforos binários e de contagem.

---

## Aplicação

### `verifica_primalidade.c`

Este programa realiza o teste de primalidade de forma concorrente, usando um produtor que gera `N` números inteiros e `C` consumidores que verificam se esses números são primos.

**Funcionalidades:**

* Recebe como entrada:

  * `N`: quantidade total de números a gerar;
  * `M`: tamanho do buffer circular (canal entre produtor e consumidores);
  * `C`: quantidade de threads consumidoras.
* O produtor insere os `N` números no buffer.
* As `C` threads consumidoras retiram números do buffer e verificam se são primos.
* O programa imprime:

  * A quantidade total de números primos encontrados;
  * Qual thread consumidora encontrou mais primos ("vencedora").

**Uso:**

```bash
# Para compilar (com math e pthread):
gcc -o verifica_primalidade verifica_primalidade.c -lm -lpthread

# Para executar:
./verifica_primalidade

# Exemplo:
# N=1000, buffer M=10, com 4 consumidores
./verifica_primalidade
Digite N (quantidade de numeros a gerar): 1000
Digite M (tamanho do buffer): 10
Digite C (quantidade de consumidores): 4
```

**Exemplo de saídas esperadas:**

```
Total de primos encontrados: 168
Thread vencedora: Consumidor 2 com 56 primos
```

```
Total de primos encontrados: 168
Thread vencedora: Consumidor 0 com 61 primos
```

---

## Observações sobre a sincronização

O programa utiliza os seguintes semáforos:

| Semáforo | Finalidade                           | Valor Inicial |
| -------- | ------------------------------------ | ------------- |
| `mutex`  | Exclusão mútua na seção crítica      | 1             |
| `vazio`  | Conta os slots livres no buffer      | M             |
| `cheio`  | Conta os slots ocupados para consumo | 0             |

A estrutura de buffer circular é coordenada usando `in` e `out`, e o controle de concorrência garante que não haja sobrescritas nem leituras inválidas.

---

## [Post Mortem] 

### Problema identificado com `N == M`

Durante os testes com `N == M`, observou-se um comportamento crítico: **o resultado indicava 0 primos encontrados**. Isso acontecia porque todos os consumidores verificavam a condição:

```c
if (finalizado && in == out) {
    sem_post(&mutex);
    break;
}
```

Essa lógica é frágil no caso de `in == out` (buffer circular) mesmo com itens ainda não processados. Os consumidores podiam encerrar **antes de realmente consumir os dados**, resultando em 0 primos.

---

### Correção implementada: uso de `total_consumido`

Para evitar esse erro, foi adicionado um contador global:

```c
int total_consumido = 0;
```

Esse contador é incrementado **toda vez que um consumidor consome um item**. A nova lógica para encerrar as threads consumidoras ficou assim:

```c
if (finalizado && total_consumido >= N) {
    sem_post(&mutex);
    break;
}
```

Isso garante que **todos os N números gerados tenham sido realmente processados antes de qualquer consumidor terminar**.

---

## Considerações sobre desempenho

* Em execuções com muitos consumidores (ex: C = 1000), consumidores criados primeiro (como o consumidor 0) tendem a vencer mais vezes.
* Isso ocorre por:

  * Prioridade de escalonamento;
  * Menor contenção nos primeiros ciclos;
  * Acesso precoce ao mutex e ao buffer.
* A justiça entre as threads poderia ser balanceada com:

  * Sorteio da ordem de criação das threads;
  * Pequeno `usleep()` inicial aleatório, para evitar que:
    * todas as threads consumidoras sejam executadas ao mesmo tempo
    * embaralhar a ordem de acesso ao buffer
    * reduzir a vantagem de threads criadas antes

---

## Possíveis próximas melhorias

* Medir o tempo total de execução.
* Testar escalabilidade variando `C` e fixando `N`.
* Implementar histórico de quantos números cada thread verificou.
* Ajuste de balanceamento entre threads (redução de vantagem).

---

## Conclusões

* O padrão produtor/consumidor com semáforos é eficaz para controle concorrente.
* A condição `finalizado && in == out` é frágil sozinha em buffers circulares.
* A correção usando `total_consumido` garantiu que todos os itens fossem realmente processados.
* O programa passou a retornar resultados **estáveis**, mesmo com `N == M` e `C` elevado.

---
