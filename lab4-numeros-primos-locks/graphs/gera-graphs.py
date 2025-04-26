import matplotlib.pyplot as plt
import glob

dados = {}

for filename in glob.glob('../resultados_N*threads.txt'):
    partes = filename.split('_')
    N = int(partes[1][1:])
    threads = int(partes[2].replace('threads.txt', ''))

    with open(filename, 'r') as f:
        linhas = f.readlines()
        tempo = float(linhas[0].strip())
        aceleracao = float(linhas[1].strip())
        eficiencia = float(linhas[2].strip())

    if N not in dados:
        dados[N] = {'threads': [], 'tempo': [], 'aceleracao': [], 'eficiencia': []}

    dados[N]['threads'].append(threads)
    dados[N]['tempo'].append(tempo)
    dados[N]['aceleracao'].append(aceleracao)
    dados[N]['eficiencia'].append(eficiencia)

for N in sorted(dados.keys()):
    threads = sorted(dados[N]['threads'])
    tempos = [x for _, x in sorted(zip(dados[N]['threads'], dados[N]['tempo']))]
    aceleracoes = [x for _, x in sorted(zip(dados[N]['threads'], dados[N]['aceleracao']))]
    eficiencias = [x for _, x in sorted(zip(dados[N]['threads'], dados[N]['eficiencia']))]

    plt.figure()
    plt.plot(threads, tempos, marker='o')
    plt.title(f"Tempo de execução vs Número de Threads (N={N})")
    plt.xlabel("Número de Threads")
    plt.ylabel("Tempo (segundos)")
    plt.grid(True)
    plt.savefig(f"grafico_tempo_N{N}.png")

    plt.figure()
    plt.plot(threads, aceleracoes, marker='o')
    plt.title(f"Aceleração vs Número de Threads (N={N})")
    plt.xlabel("Número de Threads")
    plt.ylabel("Aceleração")
    plt.grid(True)
    plt.savefig(f"grafico_aceleracao_N{N}.png")

    plt.figure()
    plt.plot(threads, eficiencias, marker='o')
    plt.title(f"Eficiência vs Número de Threads (N={N})")
    plt.xlabel("Número de Threads")
    plt.ylabel("Eficiência")
    plt.grid(True)
    plt.savefig(f"grafico_eficiencia_N{N}.png")

print("Gráficos gerados com sucesso!")
