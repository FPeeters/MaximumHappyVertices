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
            for i in range(nbColors):
                G.add_node(i)
        elif split[0] == 'e':
            n1 = int(split[1]) - 1
            n2 = int(split[2]) - 1
            G.add_edge(n1, n2)
        elif split[0] == 'n':
            nbPrecolor += 1

    return G.number_of_nodes(), G.number_of_edges(), nx.density(G), 2* G.number_of_edges() / float(G.number_of_nodes()),
            

tunedArgs = ["-a", "simAnn", "-init", "growth", "-temp", "51", "-swap", "0.06", "-split", "0.06"]

file = open("results.txt", "w", newline="")
writer = csv.writer(file)

nbNodes_options = [500, 1000, 1500, 2000]
nbColor_options = [5, 10, 15, 20]
preColor_options = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40]
edgeProb_options = [0.012, 0.013, 0.0135, 0.014, 0.0145]
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
                    t = time.time()
                    filename = "todo/graph" + str(count) + ".txt"
                    gen_result = subprocess.run(["cmake-build-visual-studio\\generator.exe",
                                                 "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                                                 "-R", str(edgeProb), "-s", str(seed), "-f", filename],
                                                stdout=subprocess.PIPE, universal_newlines=True)
                    greedy_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)
                    growth_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename,
                                                    "-a", "growth"], stdout=subprocess.PIPE, universal_newlines=True)
                    sim_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename] + tunedArgs,
                                                stdout=subprocess.PIPE, universal_newlines=True)
                    sim_result.check_returncode()

                    os.remove(filename)

                    gen = gen_result.stdout.split("\t")
                    greedy = greedy_result.stdout.split("\n")[-1]
                    growth = growth_result.stdout.split("\n")[-1]
                    sim = sim_result.stdout.split("\n")[-1]

                    writer.writerow(gen[1:4] + gen[7:8] + gen[5:7] + [greedy, growth, sim])

                    if avgTime == -1:
                        avgTime = time.time() - t
                    else:
                        avgTime = (time.time() - t) * emaFactor + avgTime * (1 - emaFactor)

                    count += 1
                    print_progress(count, nbGraphs, avgTime)

file.close()
