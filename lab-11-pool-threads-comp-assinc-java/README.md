# Verificação de Números Primos com *threads* em Java ☕️

Este experimento da disciplina **Programação Concorrente (UFRJ 25.1)** tem como objetivo comparar duas abordagens diferentes para paralelizar a verificação de números primos:

- Uma utilizando **`ExecutorService` com `Future` e `Callable`**, para computação assíncrona (atividade 3 — itens 2 e 3).
- Outra implementando manualmente um **pool de threads com fila de tarefas**, controlando diretamente os fluxos de execução (atividade 1 — item 4).

---

## Arquivos

### `FutureHello.java`

Este programa calcula quantos números primos existem no intervalo de `1` a `10.000`, de forma **paralela e assíncrona**, utilizando:

- A interface `Callable`, que permite retorno de valores (neste caso, `Boolean`).
- Um `ExecutorService` com `N` threads para distribuir os testes de primalidade.
- Uma lista de `Future<Boolean>`, onde cada tarefa analisa um número individualmente.
- Por fim, conta quantos retornaram `true` e imprime o total de primos encontrados.

**Exemplo de uso:**

```bash
javac FutureHello.java
java FutureHello
```

#### ✅ Exemplo de Saída

```
O número 3 é primo
O número 5 é primo
O número 10 não é primo
O número 12 não é primo
O número 8 não é primo
O número 14 não é primo
O número 15 não é primo
O número 1 não é primo
O número 9 não é primo
O número 2 é primo
O número 7 é primo
O número 0 não é primo
...
O número 9940 não é primo
O número 9941 é primo
O número 9970 não é primo
O número 9977 não é primo
O número 9978 não é primo
O número 9987 não é primo
O número 9996 não é primo
Total de primos entre 1 e 10000: 1229
```

---

### `MyPool.java`

Este programa implementa uma **pool de threads customizada** chamado `FilaTarefas`, baseado em uma fila sincronizada:

- Cada thread do pool consome tarefas da fila (`Runnable`).
- As tarefas consistem em verificar se um número é primo e imprimir o resultado.
- O pool é finalizado com `shutdown()` após todas as tarefas serem enviadas.

**Exemplo de uso:**

```bash
javac MyPool.java
java MyPool
```

#### ✅ Exemplo de Saída

```
O número 4 não é primo
O número 10 não é primo
O número 11 é primo
O número 12 não é primo
O número 13 é primo
O número 14 não é primo
O número 15 não é primo
O número 16 não é primo
O número 17 é primo
O número 18 não é primo
O número 19 é primo
O número 20 não é primo
O número 21 não é primo
O número 22 não é primo
O número 23 é primo
O número 24 não é primo
O número 0 não é primo
O número 1 não é primo
O número 9 não é primo
O número 6 não é primo
O número 5 é primo
O número 3 é primo
O número 8 não é primo
O número 7 é primo
O número 2 é primo
Terminou
```

---

---

## Conclusões

Essas duas abordagens ilustram como é possível:

- Criar **computações concorrentes e distribuídas** de forma simples com `ExecutorService`.
- **Controlar diretamente** o funcionamento de um pool de threads manualmente, o que é útil para entender como funciona um agendador básico.
- Explorar o conceito de **paralelismo embarcado em tarefas independentes**, como a verificação de números primos.

Além disso, o uso de `Future` permite **coletar resultados de tarefas individuais**, enquanto o pool customizado é útil para aplicações onde só se deseja realizar tarefas sem esperar retorno.
