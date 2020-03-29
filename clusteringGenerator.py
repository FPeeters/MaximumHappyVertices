import random
import math
import subprocess
import os


def _linear_distr(alpha):
    return (math.sqrt(alpha * alpha - 2 * alpha + 4 * alpha * random.uniform(0, 1) + 1) - 1) / alpha


def linear_distr(alpha):
    alpha = 2 if alpha > 2 else alpha
    alpha = -2 if alpha < -2 else alpha

    if alpha == 0:
        return random.uniform(0, 1)
    elif alpha > 1:
        return ((_linear_distr(1) + 1) * (2 - alpha) + 2 * alpha - 2) / 2
    elif alpha < -1:
        return (_linear_distr(-1) + 1) * (alpha + 2) / 2.
    else:
        return (_linear_distr(alpha) + 1) / 2.


def generate_graph(seed, nbNodes, density, cluster, nbColors, precolorRatio):
    nbPrecolor = round(nbNodes * precolorRatio)
    if nbPrecolor < nbColors:
        print("At least", nbColors, "nodes have to be precolored.")
        return [[]]

    random.seed(seed)

    adjacency = [[i, [], 0] for i in range(nbNodes)]
    nbEdges = round(nbNodes * (nbNodes - 1) * density / 2)

    nbFilled = 0
    for _ in range(nbEdges):
        current_range = nbNodes - nbFilled
        first = round(linear_distr(cluster) * current_range - 0.5)
        if first == current_range:
            first -= 1

        current_range -= 1
        second = round(linear_distr(cluster) * current_range - 0.5)
        if second == current_range:
            second -= 1
        if second >= first:
            second += 1

        direction = 1 if cluster > 0 else -1
        start = second
        while second == first or adjacency[second][0] in adjacency[first][1]:
            second += direction
            if second == nbNodes:
                second = start - 1
                direction = -1
            elif second == -1:
                second = start + 1
                direction = 1

        adjacency[first][1].append(adjacency[second][0])
        adjacency[second][1].append(adjacency[first][0])
        if len(adjacency[first][1]) == nbNodes - 1:
            nbFilled += 1
        if len(adjacency[second][1]) == nbNodes - 1:
            nbFilled += 1

        random.shuffle(adjacency)
        adjacency.sort(key=lambda n: len(n[1]))

    renames = []
    for row in adjacency:
        renames.append(row[0])
    for row in adjacency:
        row[0] = nbNodes - renames.index(row[0])
        row[1] = [nbNodes - renames.index(x) for x in row[1]]

    random.shuffle(adjacency)
    for col in range(nbColors):
        adjacency[col][2] = col + 1
    for i in range(nbColors, nbPrecolor):
        adjacency[i][2] = random.randint(1, nbColors)
    adjacency.sort(key=lambda n: -len(n[1]))

    return adjacency


def generate_image(filename, adjacency):
    file = open(filename + ".dot", "w+")
    file.write("graph {\n")
    file.write("graph [overlap=false]\n")
    file.write("node [style=filled,width=0.05,label=\"\",fixedsize=true]\n")
    for node in adjacency:
        file.write("\tn" + str(node[0]) + "[shape=circle]\n")
    for node in adjacency:
        file.write("\tn" + str(node[0]) + " -- {" + ",".join(
            map(lambda x: "n" + str(x), filter(lambda x: x < node[0], node[1]))) + "}\n")
    file.write("}\n")
    file.close()
    subprocess.run(["sfdp", filename + ".dot", "-Tpng", "-o", filename])
    os.remove(filename + ".dot")


def write_to_file(filename, adjacency, nbNodes, density, nbColors):
    file = open(filename, "w+")

    file.write("p edge " + str(nbNodes) + " " + str(round(nbNodes * (nbNodes - 1) * density / 2)) + " " + str(nbColors) + "\n")

    for row in adjacency:
        for adj in row[1]:
            if row[0] < adj:
                file.write("e " + str(row[0]) + " " + str(adj) + "\n")

    for row in adjacency:
        if row[2] != 0:
            file.write("n " + str(row[0]) + " " + str(row[2]) + "\n")

    file.close()


if __name__ == '__main__':
    import time
    import matplotlib.pyplot as plt

    # for alpha in [-1.75, -1.25, -0.5, 0, 0.5, 1.25, 1.75]:
    #     a = [0 for _ in range(100)]
    #     for _ in range(100_000):
    #         x = linear_distr(-alpha)
    #         a[round(x*100 - 0.5)] += 1
    #     plt.plot(a)
    # plt.show()

    graph = generate_graph(1, 10, 0.2, 0, 5, 0.7)
    print(graph)
    generate_image("out.png", graph)
    write_to_file("graph.txt", graph, 10, 0.2, 5)

    # for i in [0., 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 1.8, 1.9, 2.]:
    #     t = time.time()
    #     graph = generate_graph(1, 20, 0.1, i)
    #     print(time.time() - t)
    #     plt.plot([len(x[1]) for x in graph])
    #     generate_image("plots/out" + str(i*4 + 8) + ".png", graph)
    #     t = time.time()
    #     graph = generate_graph(1, 20, 0.1, -i)
    #     print(time.time() - t)
    #     plt.plot([len(x[1]) for x in graph])
    #     generate_image("plots/out" + str(-i*4 + 8) + ".png", graph)
    # plt.show()
