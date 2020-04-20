import matplotlib.pyplot as plt
import math
import numpy as np

file = open("growthSeedResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines()]
file.close()

nbBuckets = 30

results = [x for x in results if x[-1] != 0 and x[4] * (x[0] - 1.) < 30]

x = [y[5] * (y[0] - 1.) for y in results]
lewis = [y[6] for y in results]
average = [y[7] for y in results]
std = [y[8] for y in results]
zscore = [(y[6] - y[7]) / y[8] if y[8] != 0 else 0 for y in results]
best = [y[9] for y in results]

buckets = [[0., 0., 0., 0., 0] for _ in range(nbBuckets)]
for j in range(len(x)):
    index = math.floor((x[j] - min(x))/max(x)*nbBuckets)
    if index == nbBuckets:
        buckets[-1][0] += average[j]
        buckets[-1][1] += best[j]
        buckets[-1][2] += lewis[j]
        buckets[-1][3] += std[j]
        buckets[-1][4] += 1
    else:
        buckets[index][0] += average[j]
        buckets[index][1] += best[j]
        buckets[index][2] += lewis[j]
        buckets[index][3] += std[j]
        buckets[index][4] += 1
indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
buckets = [[y[0] / y[4], y[1] / y[4], y[2] / y[4], y[3] / y[4]] if y[4] != 0 else 0 for y in buckets]

plt.figure()
plt.errorbar(indices, [x[0] for x in buckets], yerr=[x[3] for x in buckets], label="Average")
plt.plot(indices, [x[1] for x in buckets], label="Best")
plt.plot(indices, [x[2] for x in buckets], label="Lewis")

plt.legend()
plt.ylabel("nbHappy")
plt.xlabel("Average degree")
plt.savefig("plots/growth_seed.png")
plt.close()
