import matplotlib.pyplot as plt
import math

file = open("500comparison.txt", "r")
results = [list(map(lambda x: float(x), line.split(','))) for line in file.readlines()]
file.close()

file = open("graphFeatures.txt", "r")
features = [list(map(lambda x: float(x), line.split(',')))[4:] for line in file.readlines()]
file.close()

for i in range(len(results)):
    features[i][-1] = results[i][2] / results[i][0]

happy = [x[6:9] for x in results]

# colorFilter = [1 if 0.002 < x[2] < 0.013 else 0 for x in features]

feature_names = ["Number of nodes", "Number of edges", "Density", "Average degree", "Std of degree",
                 "Average shortest path length", "Diameter", "Minimum cylce length", "Average centrality",
                 "Std of centrality", "Average clustering", "Wiener index", "Average absolute eigenvalue",
                 "Std of eienvalues", "Algebraic connectivity", "Average eigenvector centrality",
                 "Std of eigenvector centrality", "Number of Colors", "Proportion of precolored vertices"]
nbBuckets = 25

for i in range(len(features[0])):
    plt.figure()
    x = [y[i] for y in features]

    if max(x) == 0 or not math.isfinite(max(x)):
        continue

    buckets = [[0., 0., 0., 0] for _ in range(nbBuckets)]
    for j in range(len(x)):
        index = math.floor((x[j] - min(x))/max(x)*nbBuckets)
        if index == nbBuckets:
            buckets[-1][0] += happy[j][0]
            buckets[-1][1] += happy[j][1]
            buckets[-1][2] += happy[j][2]
            buckets[-1][3] += 1
        else:
            buckets[index][0] += happy[j][0]
            buckets[index][1] += happy[j][1]
            buckets[index][2] += happy[j][2]
            buckets[index][3] += 1
    indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
    buckets = [[y[0] / y[3], y[1] / y[3], y[2]/y[3]] if y[3] != 0 else [0., 0., 0.] for y in buckets]
    buckets = [[z/max(y) if max(y) != 0 else 0. for z in y] for y in buckets]

    plt.axis([0, max(x), 0, 1.1])
    # plt.scatter(x, diff, marker='.') # , c=colorFilter
    plt.plot(indices, buckets)
    plt.ylabel("Average performance")
    plt.xlabel(feature_names[i])
    plt.legend(["greedy", "growth", "simulated annealing"])
    plt.savefig("plots/construct_" + feature_names[i] + ".png")
    plt.close()
