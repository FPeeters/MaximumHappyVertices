import matplotlib.pyplot as plt
import math

file = open("reduceResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]
file.close()

nbBuckets = 100

results = [y for y in results if y[5] < 100 / (y[0] - 1.)]

for precolor in [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40]:
    x = [y[5] * (y[0] - 1.) for y in results if y[2] == precolor and y[1] == 5]
    thiriv_reduce = [y[6] for y in results if y[2] == precolor and y[1] == 5]
    basic_reduce = [y[7] for y in results if y[2] == precolor and y[1] == 5]
    articul_reduce = [y[8] for y in results if y[2] == precolor and y[1] == 5]

    buckets = [[0., 0., 0., 0] for _ in range(nbBuckets)]
    for j in range(len(x)):
        index = math.floor((x[j] - min(x))/max(x)*nbBuckets)
        if index == nbBuckets:
            buckets[-1][0] += thiriv_reduce[j]
            buckets[-1][1] += basic_reduce[j]
            buckets[-1][2] += articul_reduce[j]
            buckets[-1][3] += 1
        else:
            buckets[index][0] += thiriv_reduce[j]
            buckets[index][1] += basic_reduce[j]
            buckets[index][2] += articul_reduce[j]
            buckets[index][3] += 1
    indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
    buckets = [[y[0] / y[3], y[1] / y[3], y[2] / y[3]] if y[3] != 0 else 0 for y in buckets]

    plt.figure()
    plt.plot(indices, buckets)

    plt.legend(["Thiruvady", "Basic", "Articulation"])
    plt.ylabel("Nb of nodes reduced")
    plt.xlabel("Average degree")
    plt.savefig("plots/reduce_" + str(precolor*2) + ".png")
    plt.close()
