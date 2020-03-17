import random
import math
import subprocess
import os


def acute_linear(alpha):
    if alpha == 0:
        acute = random.uniform(0, 1)
    elif alpha > 1:
        cutoff = 2 * alpha - 3
        acute = math.sqrt(4 * random.uniform(0, 1)) - 1
        acute = (acute + 1) / 2.
        acute = acute * (1 - cutoff) + cutoff
        acute = (acute + 1) / 2.
    elif alpha < -1:
        cutoff = 2 * alpha + 3
        acute = -math.sqrt(4 - 4 * random.uniform(0, 1)) + 1
        acute = (acute + 1) / 2.
        acute = acute * (cutoff + 1) - 1
        acute = (acute + 1) / 2.
    else:
        acute = (math.sqrt(alpha * alpha - 2 * alpha + 4 * alpha * random.uniform(0, 1) + 1) - 1) / alpha
        acute = (acute + 1) / 2.
    return acute


def generate_graph(seed, nbNodes, density, cluster):
    random.seed(seed)

    adjacency = [[i, []] for i in range(nbNodes)]
    nbEdges = round(nbNodes * (nbNodes - 1) * density / 2)

    random.shuffle(adjacency)

    nbFilled = 0
    for _ in range(nbEdges):
        current_range = nbNodes - nbFilled
        first = round(acute_linear(cluster) * current_range - 0.5)
        if first == current_range:
            first -= 1

        current_range -= 1
        second = round(acute_linear(cluster) * current_range - 0.5)
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


if __name__ == '__main__':
    # import time
    # import matplotlib.pyplot as plt

    # for alpha in [-1.75, -1.25, -0.5, 0, 0.5, 1.25, 1.75]:
    #     a = [0 for _ in range(100)]
    #     for _ in range(100_000):
    #         x = acuteLinear(100, -alpha)
    #         a[x] += 1
    #     plt.plot(a)
    # plt.show()

    graph = generate_graph(1, 10, 0.2, 2)
    print(graph)
    generate_image("out.png", graph)

    # for i in [0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 1.8, 1.9, 2]:
    #     t = time.time()
    #     graph = generateGraph(1, 200, 0.1, i)
    #     print(time.time() - t)
    #     plt.plot([len(x[1]) for x in graph])
    #     generateImage("plots/out" + str(i+8) + ".png", graph)
    # plt.show()
