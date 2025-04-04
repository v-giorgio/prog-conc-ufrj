# Produto interno com *threads* em C ⚙️

Esta atividade da disciplina Programação Concorrente (UFRJ 25.1) tem como objetivo mostrar, de forma prática, como calcular o **produto interno** entre dois vetores de forma **sequencial** e também **paralela**, utilizando **threads**, em C.

---

## 🧪 Arquivos

### `gera_vetores.c`
Este programa gera dois vetores de números `float` com dimensão `N`, preenchidos com valores aleatórios, e calcula o **produto interno** entre eles de forma sequencial. 

Depois disso, ele salva em um arquivo binário (para não haver perda de precisão):
- o tamanho dos vetores
- os dois vetores
- e o valor do produto interno calculado de forma sequencial

em um arquivo binário.

**Uso:**

```bash
# Para compilar o arquivo (tenha o gcc compiler baixado - nativo, caso use Unix-based OS):
gcc -o gera_vetores gera_vetores.c
```

```bash
./gera_vetores <dimensao> <nome_do_arquivo>

# Exemplo:
./gera_vetores 10 vetores
```

### `calcula_prod_interno.c`

Este programa calcula o produto interno entre dois vetores utilizando **múltiplas threads**. Ele faz isso da seguinte forma:

- Lê um arquivo binário previamente gerado por `gera_vetores.c`, que contém dois vetores de `float`, o tamanho desses vetores e o valor do produto interno calculado sequencialmente.
- Divide os pares de elementos dos vetores entre as threads, de forma balanceada.
- Cada thread calcula uma soma parcial do produto interno (multiplicando os elementos correspondentes e somando).
- A **thread principal** coleta os resultados parciais e soma tudo para obter o produto final.
- Por fim, compara esse resultado com o valor original (salvo no arquivo) e imprime o erro percentual.

#### Como usar:

```bash
gcc -o calcula_prod_interno calcula_prod_interno.c
```

```bash
./calcula_prod_interno <arquivo_vetores> <numero_threads>

# Exemplo:
./calcula_prod_interno vetores 20
```

---

## ✅ Resultado Obtido

```bash
Soma original (do arquivo): -1001.77777099609375000000000000
Soma com threads:          -1001.77776580386671412270516157
Erro percentual:           0.00000051830128260028448811%
```

![Evidências resultado](result.png)

---

## 📌 Conclusões

Apesar das somas darem resultados quase idênticos, há uma pequena diferença de precisão nas casas decimais. Essa diferença é esperada quando lidamos com operações de ponto flutuante em paralelo.

Cada thread faz contas de multiplicação com floats e depois soma os resultados — a ordem de execução muda e isso impacta um pouquinho o resultado final. Mas o erro foi tão pequeno (0.0000005%) que é totalmente aceitável para fins práticos.

Esse tipo de comportamento mostra como operações com float e double não são sempre precisas em larga escala, mas mostra o potencial de usar threads para paralelizar o trabalho.