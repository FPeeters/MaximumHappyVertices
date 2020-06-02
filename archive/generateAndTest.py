import subprocess
import csv
import os
import time
import numpy as np
import networkx as nx
from multiprocessing import Pool, Lock

import clusteringGenerator


# Adapted from https://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
def print_progress(iteration, total, avg_time=0):
    if avg_time < 0:
        avg_time = 0
    filledLength = int(50 * iteration // total)
    bar = '█' * filledLength + '-' * (50 - filledLength)
    avg_time_str = str(round(avg_time)) + "s"
    time_remaining = '{0:02.0f}:{1:02.0f}'.format(*divmod(avg_time * (total - iteration) / threads, 60))
    print('\r|%s| %s avg: %s eta: %s ' % (bar, iteration, avg_time_str, time_remaining), end="")
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
        np.fromiter(map(lambda x: abs(x), eigenvalues), dtype=float)), np.std(eigenvalues), -1, \
           np.mean(eigenvector_centrality), np.std(eigenvector_centrality), \
           nbColors, nbPrecolor / float(G.number_of_nodes())


def run_instance(filename, nbNodes, nbColors, preColor, degree, cluster, seed):
    t = time.time()
    gen_result = subprocess.run(["cmake-build-visual-studio\\generator.exe",
                                 "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                                 "-R", str(degree / (nbNodes - 1.)), "-s", str(seed), "-f", filename],
                                stdout=subprocess.PIPE, universal_newlines=True)

    # graph = clusteringGenerator.generate_graph(seed, nbNodes, degree / (nbNodes - 1.), cluster, nbColors, preColor)
    # clusteringGenerator.write_to_file(filename, graph, nbNodes, degree / (nbNodes - 1.), nbColors)
    # gen = [nbNodes, nbColors, preColor, seed, degree, cluster]

    # features = calculate_features(filename)

    greedy_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)
    growth_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                    "-a", "growth"], stdout=subprocess.PIPE, universal_newlines=True)
    sim_result = []
    for args in tunedArgs:
        sim_result.append(subprocess.run(["cmake-build-visual-studio\\main.exe", filename] + args,
                                     stdout=subprocess.PIPE, universal_newlines=True))

    tabu_result = subprocess.run(["cmake-build-visual-studio\\happyTabu.exe", filename, "-t", "30"],
                                 stdout=subprocess.PIPE, universal_newlines=True)
    # exact_t = time.time_ns()
    # exact_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename, "-a", "exact",
    #                                "-threads", "6"], stdout=subprocess.PIPE, universal_newlines=True)
    # exact_t = (time.time_ns() - exact_t) / 1_000_000.
    os.remove(filename)

    gen = gen_result.stdout.split("\t")
    gen[3] = str(float(gen[3]) / nbNodes)
    gen = gen[1:4] + gen[7:8] + gen[5:7]
    greedy = greedy_result.stdout.split("\n")[-1]
    growth = growth_result.stdout.split("\n")[-1]
    sim = [x.stdout.split("\n")[-1] for x in sim_result]
    # exact = exact_result.stdout.split("\n")[-1]
    tabu = tabu_result.stdout.split("\n")[-2].split("\t")[8]

    return gen + [greedy, growth] + sim + [tabu], time.time() - t # exact, exact_t


def callback(result):
    global avgTime, count, lock
    lock.acquire()
    if avgTime == -1:
        avgTime = result[1]
    else:
        avgTime = result[1] * emaFactor + avgTime * (1 - emaFactor)
    writer.writerow(result[0])
    count += 1
    print_progress(count, nbGraphs, avgTime)
    lock.release()


count = 0
avgTime = -1
emaFactor = 0
tunedArgs = [["-a", "simAnn", "-maxI", "5000",   "-init", "growth", "-temp", "255", "-swap", "0.37", "-split", "0.22", "-zeroTemp", "0.93"],
             ["-a", "simAnn", "-maxI", "20000",  "-init", "growth", "-temp", "361", "-swap", "0.28", "-split", "0.58", "-zeroTemp", "0.52"],
             ["-a", "simAnn", "-maxI", "50000",  "-init", "greedy", "-temp", "222", "-swap", "0.19", "-split", "0.50", "-zeroTemp", "0.56"],
             ["-a", "simAnn", "-maxI", "50000",  "-init", "random", "-temp", "152", "-swap", "0.26", "-split", "0.47", "-zeroTemp", "0.92"],
             ["-a", "simAnn", "-maxI", "100000", "-init", "best",   "-temp", "332", "-swap", "0.15", "-split", "0.38", "-zeroTemp", "0.74"]]
threads = 7
lock = Lock()

if __name__ == '__main__':
    file = open("results.txt", "w", newline="")
    writer = csv.writer(file)

    nbNodes_options = [1000]
    nbColor_options = [5, 10, 15]
    # preColor_options = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40]
    preColor_options = [0.05, 0.15, 0.25, 0.35]
    # edgeProb_options = [0.0005, 0.001, 0.0015, 0.002, 0.0025, 0.003, 0.0035, 0.004, 0.0045, 0.005, 0.0055, 0.006,
    #                     0.0065, 0.007, 0.0075, 0.008, 0.0085, 0.009, 0.0095, 0.01, 0.0105, 0.011, 0.0115, 0.012,
    #                     0.0125, 0.013, 0.0135, 0.014, 0.0145, 0.015]
    # edgeProb_options = [x / 100. for x in range(0, 101, 2)]
    degree_options = [float(x) for x in range(1, 30)]
    # cluster_options = [-2, -1.5, -1., -.5, 0, .5, 1., 1.5, 2]
    cluster_options = [0]
    seed_options = [1234, 4321]

    nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * \
               len(degree_options) * len(cluster_options) * len(seed_options)
    emaFactor = 2. / (nbGraphs / (len(nbNodes_options) + 1.))

    print("Total graphs:", nbGraphs)
    print_progress(0, nbGraphs)

    pool = Pool(threads)

    fileCount = 0
    for nbNodes in nbNodes_options:
        for nbColors in nbColor_options:
            for preColor in preColor_options:
                for degree in degree_options:
                    for cluster in cluster_options:
                        for seed in seed_options:
                            fileCount += 1
                            if preColor * nbNodes < nbColors:
                                writer.writerow([nbNodes, nbColors, nbNodes * preColor, seed])
                                count += 1
                                print_progress(count, nbGraphs, avgTime)
                                continue

                            filename = "todo/graph" + str(fileCount) + ".txt"
                            pool.apply_async(run_instance,
                                             (filename, nbNodes, nbColors, preColor, degree, cluster, seed),
                                             callback=callback)

    pool.close()
    pool.join()
    file.close()
