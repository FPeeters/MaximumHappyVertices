import matplotlib.pyplot as plt
import math

file = open("exactResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]
# results = [x for x in results if x[-1] != 0]
file.close()

exactTime = [x[24] / 1000. for x in results]

# colorFilter = [1 if 0.002 < x[2] < 0.013 else 0 for x in features]

feature_names = ["Number of nodes", "Number of edges", "Density", "Average degree", "Std of degree",
                 "Average shortest path length", "Diameter", "Minimum cylce length", "Average centrality",
                 "Std of centrality", "Average clustering", "Wiener index", "Average absolute eigenvalue",
                 "Std of eienvalues", "Algebraic connectivity", "Average eigenvector centrality",
                 "Std of eigenvector centrality", "Number of Colors", "Proportion of precolored vertices"]
nbBuckets = 25

for i in range(len(feature_names)):
    plt.figure()
    x = [y[i + 4] for y in results]

    if max(x) == 0 or not math.isfinite(max(x)):
        continue

    buckets = [[0., 0] for _ in range(nbBuckets)]
    for j in range(len(x)):
        index = math.floor((x[j] - min(x))/max(x)*nbBuckets)
        if index == nbBuckets:
            buckets[-1][0] += exactTime[j]
            buckets[-1][1] += 1
        else:
            buckets[index][0] += exactTime[j]
            buckets[index][1] += 1
    indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
    buckets = [y[0] / y[1] if y[1] != 0 else 0 for y in buckets]

    plt.axis([0, max(x), 0, max(buckets) + 5])
    # plt.scatter(x, exactTime, marker='.')
    plt.ylabel("Average execution time (ms)")
    plt.xlabel(feature_names[i])
    plt.plot(indices, buckets)
    plt.savefig("plots/exact_" + feature_names[i] + ".png")
    plt.close()
