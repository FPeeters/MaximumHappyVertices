import matplotlib.pyplot as plt
import math
import numpy as np

file = open("growthSeedResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines()]
file.close()

nbBuckets = 40

results = [x for x in results if x[-1] != 0]

x = [y[5] * (y[0] - 1.) for y in results]
lewis = [y[6] for y in results]
average = [y[6] - y[7] for y in results]
zscore = [(y[6] - y[7]) / y[8] if y[8] != 0 else 0 for y in results]
best = [y[6] - y[9] for y in results]

print(np.average(average))
print(np.average(best))

buckets = [[0., 0., 0., 0] for _ in range(nbBuckets)]
for j in range(len(x)):
    index = math.floor((x[j] - min(x))/max(x)*nbBuckets)
    if index == nbBuckets:
        buckets[-1][0] += average[j]
        buckets[-1][1] += best[j]
        buckets[-1][2] += zscore[j]
        buckets[-1][3] += 1
    else:
        buckets[index][0] += average[j]
        buckets[index][1] += best[j]
        buckets[index][2] += zscore[j]
        buckets[index][3] += 1
indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
buckets = [[y[0] / y[3], y[1] / y[3], y[2] / y[3]] if y[3] != 0 else 0 for y in buckets]

plt.figure()
plt.plot(indices, [x[:2] for x in buckets])

plt.legend(["Default vs. average", "Default vs. best"])
plt.ylabel("Difference in nbHappy")
plt.xlabel("Average degree")
plt.savefig("plots/growth_seed.png")
plt.close()

plt.figure()
plt.plot(indices, [x[2] for x in buckets])

plt.ylabel("z-score")
plt.xlabel("Average degree")
plt.savefig("plots/growth_seed_zscore.png")
plt.close()
