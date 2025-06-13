/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

/* Aluno: Vitor Lucio Giorgio */
/* Atividade 1 - item 4 */
/* -------------------------------------------------------------------*/

import java.util.LinkedList;

import static java.lang.Math.sqrt;


class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}

class PrimoRunnable implements Runnable {
    Integer primo;

    String ehPrimoOuNao = "O número %d %sé primo";

    public PrimoRunnable(Integer primo) {
        this.primo = primo;
    }

    private void imprime(Boolean ehPrimo) {
        if (ehPrimo) System.out.printf((ehPrimoOuNao) + "%n", this.primo, "");
        else System.out.printf((ehPrimoOuNao) + "%n", this.primo, "não ");
    }

    public void run() {
        if (this.primo == 2) { imprime(true); return; }

        if (this.primo <= 1 || this.primo % 2 == 0) { imprime(false); return; }

        for (int i = 3; i < sqrt(this.primo) + 1; i+= 2) {
            if (this.primo % i == 0) { imprime(false); return; }
        }

        imprime(true);
    }
}

class MyPool {
    private static final int NTHREADS = 10;

    public static void main (String[] args) {
      FilaTarefas pool = new FilaTarefas(NTHREADS); 

      for (int i = 0; i < 25; i++) {
        Runnable primo = new PrimoRunnable(i);
        pool.execute(primo);
      }

      pool.shutdown();
      System.out.println("Terminou");
   }
}
