import subprocess
import csv
import os
import time
import numpy as np
import networkx as nx


# Adapted from https://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
def print_progress(iteration, total, avg_time=0):
    percent = "{0:.0f}".format(100 * (iteration / float(total)))
    filledLength = int(50 * iteration // total)
    bar = '█' * filledLength + '-' * (50 - filledLength)
    avg_time_str = str(round(avg_time)) + "s"
    time_remaining = '{0:02.0f}:{1:02.0f}'.format(*divmod(avg_time * (total - iteration), 60))
    print('\r|%s| %s%% avg: %s eta: %s' % (bar, percent, avg_time_str, time_remaining), end="")
    if iteration == total:
        print()


def calculate_features(filename):
    graph = open(filename, "r")

    G = nx.Graph()
    nbColors = 0
    nbPrecolor = 0

    for line in graph:
        split = line.split(' ')
        if split[0] == 'p':
            nbColors = int(split[4])
            for i in range(int(split[2])):
                G.add_node(i)
        elif split[0] == 'e':
            n1 = int(split[1]) - 1
            n2 = int(split[2]) - 1
            G.add_edge(n1, n2)
        elif split[0] == 'n':
            nbPrecolor += 1
    graph.close()

    degrees = np.array(G.degree)
    centrality = np.fromiter(nx.betweenness_centrality(G).values(), dtype=int)
    eigenvalues = np.linalg.eigvals(nx.to_numpy_matrix(G))
    try:
        eigenvector_centrality = np.fromiter(nx.eigenvector_centrality(G).values(), dtype=float)
    except nx.PowerIterationFailedConvergence:
        eigenvector_centrality = np.array([0.])
    cycles = list(map(lambda x: len(x), nx.cycle_basis(G)))
    components = [nx.subgraph(G, comp) for comp in nx.connected_components(G)]

    return G.number_of_nodes(), G.number_of_edges(), nx.density(G), np.mean(degrees, axis=0)[1], \
           np.std(degrees, axis=0)[1], sum([nx.average_shortest_path_length(g) for g in components]), \
           sum([nx.diameter(g) for g in components]), "inf" if len(cycles) == 0 else min(cycles), np.mean(
        centrality), np.std(centrality), nx.average_clustering(G), nx.wiener_index(G), np.mean(
        np.fromiter(map(lambda x: abs(x), eigenvalues), dtype=float)), np.std(eigenvalues), \
           nx.linalg.algebraicconnectivity.algebraic_connectivity(G), np.mean(eigenvector_centrality), np.std(
        eigenvector_centrality), \
           nbColors, nbPrecolor / float(G.number_of_nodes())


tunedArgs = ["-a", "simAnn", "-init", "growth", "-temp", "51", "-swap", "0.06", "-split", "0.06"]

file = open("results.txt", "w", newline="")
writer = csv.writer(file)

nbNodes_options = [100, 200, 300]
nbColor_options = [5, 10, 15, 20]
preColor_options = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40]
edgeProb_options = [0.0005, 0.00075, 0.001, 0.0012, 0.0013, 0.00135, 0.0014,
                    0.00145, 0.0015, 0.002, 0.0025, 0.003, 0.0035, 0.004,
                    0.0045, 0.005, 0.0055, 0.006, 0.0065, 0.007, 0.0075,
                    0.008, 0.0085, 0.009, 0.0095, 0.01, 0.0105, 0.011,
                    0.0115, 0.012, 0.0125, 0.013, 0.0135, 0.014, 0.0145,
                    0.015]
seed_options = [1234, 4321]

count = 0
nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * len(edgeProb_options) \
           * len(seed_options)

avgTime = -1
emaFactor = 2 / (nbGraphs / len(nbNodes_options) + 1)

print("Total graphs:", nbGraphs)
print_progress(0, nbGraphs)

for nbNodes in nbNodes_options:
    for nbColors in nbColor_options:
        for preColor in preColor_options:
            for edgeProb in edgeProb_options:
                for seed in seed_options:
                    if preColor * nbNodes < nbColors:
                        writer.writerow([nbNodes, nbColors, nbNodes * preColor, seed])
                        count += 1
                        print_progress(count, nbGraphs, avgTime)
                        continue

                    t = time.time()
                    filename = "todo/graph" + str(count) + ".txt"
                    gen_result = subprocess.run(["cmake-build-visual-studio\\generator.exe",
                                                 "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                                                 "-R", str(edgeProb), "-s", str(seed), "-f", filename],
                                                stdout=subprocess.PIPE, universal_newlines=True)

                    features = calculate_features(filename)

                    greedy_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)
                    growth_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                                    "-a", "growth"], stdout=subprocess.PIPE, universal_newlines=True)
                    sim_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename] + tunedArgs,
                                                stdout=subprocess.PIPE, universal_newlines=True)
                    exact_t = time.time_ns()
                    exact_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename, "-a", "exact",
                                                   "-threads", "6"], stdout=subprocess.PIPE, universal_newlines=True)
                    exact_t = (time.time_ns() - exact_t) / 1_000.0
                    os.remove(filename)

                    gen = gen_result.stdout.split("\t")
                    greedy = greedy_result.stdout.split("\n")[-1]
                    growth = growth_result.stdout.split("\n")[-1]
                    sim = sim_result.stdout.split("\n")[-1]
                    exact = exact_result.stdout.split("\n")[-1]

                    # [greedy, growth, sim]
                    writer.writerow(gen[1:4] + gen[7:8] + list(map(lambda x: str(x), features)) +
                                    [exact, exact_t, greedy, growth, sim])
                    # writer.writerow(gen[1:4] + gen[7:8] + gen[5:7] + [greedy, growth, sim])

                    if avgTime == -1:
                        avgTime = time.time() - t
                    else:
                        avgTime = (time.time() - t) * emaFactor + avgTime * (1 - emaFactor)

                    count += 1
                    print_progress(count, nbGraphs, avgTime)

file.close()
