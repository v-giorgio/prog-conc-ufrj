/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */

/* Aluno: Vitor Lucio Giorgio */
/* Atividade 3 - itens 2 e 3 */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;

import static java.lang.Math.sqrt;

class Primo implements Callable<Boolean> {

  private final Long numero;

  String ehPrimoOuNao = "O número %d %sé primo";

  public Primo(Long numero) {
    this.numero = numero;
  }

  private void imprime(Boolean ehPrimo) {
    if (ehPrimo) System.out.printf((ehPrimoOuNao) + "%n", this.numero, "");
    else System.out.printf((ehPrimoOuNao) + "%n", this.numero, "não ");
  }

  public Boolean call() {
    if (this.numero == 2) { imprime(true); return true; }

    if (this.numero <= 1 || this.numero % 2 == 0) { imprime(false); return false; }

    for (long i = 3; i < sqrt(this.numero) + 1; i += 2) {
      if (this.numero % i == 0) { imprime(false); return false; }
    }

    imprime(true); return true;
  }
}

public class FutureHello  {
  private static final int N = 10_000;
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    List<Future<Boolean>> list = new ArrayList<Future<Boolean>>();

    for (long i = 0; i < N; i++) {
      Callable<Boolean> primoWorker = new Primo(i);
      Future<Boolean> submit = executor.submit(primoWorker);
      list.add(submit);
    }

    int totalPrimos = 0;

    for (Future<Boolean> booleanFuture : list) {
        try {
            if (booleanFuture.get()) {
                totalPrimos++;
            }
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }
    }

    System.out.println("Total de primos entre 1 e " + N + ": " + totalPrimos);

    executor.shutdown();
  }
}
