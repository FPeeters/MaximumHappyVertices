import matplotlib.pyplot as plt
import math

file = open("clusteringResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]
file.close()

nbBuckets = 15

for cluster in [-2, -1.5, -1., -.5, 0, .5, 1., 1.5, 2]:
    x = [y[4] for y in results if y[5] == cluster]
    exactTime = [y[-1] for y in results if y[5] == cluster]

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

    plt.plot(indices, buckets)

plt.legend(list(map(lambda x: "alpha=" + str(x), [-2, -1.5, -1., -.5, 0, .5, 1., 1.5, 2])))
plt.ylabel("Average execution time (ms)")
plt.xlabel("Average degree")
plt.semilogy()
plt.savefig("plots/cluster_time.png")
plt.close()
