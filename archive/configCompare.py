import matplotlib.pyplot as plt
import math

file = open("configCompare.txt", "r")
results = [list(map(lambda x: float(x), line.split(','))) for line in file.readlines()]


x = [r[5] * (r[0] - 1.) for r in results]
greedy = [r[6] for r in results]
growth = [r[7] for r in results]
config1 = [r[8] for r in results]
config2 = [r[9] for r in results]
config3 = [r[10] for r in results]
config4 = [r[11] for r in results]
config5 = [r[12] for r in results]
tabu = [r[13] for r in results]

nbBuckets = 30
buckets = [[0., 0., 0., 0., 0., 0., 0., 0., 0] for _ in range(nbBuckets)]
for j in range(len(x)):
    index = math.floor((x[j] - min(x)) / max(x) * nbBuckets)
    if index == nbBuckets:
        buckets[-1][0] += greedy[j]
        buckets[-1][1] += growth[j]
        buckets[-1][2] += config1[j]
        buckets[-1][3] += config2[j]
        buckets[-1][4] += config3[j]
        buckets[-1][5] += config4[j]
        buckets[-1][6] += config5[j]
        buckets[-1][7] += tabu[j]
        buckets[-1][8] += 1
    else:
        buckets[index][0] += greedy[j]
        buckets[index][1] += growth[j]
        buckets[index][2] += config1[j]
        buckets[index][3] += config2[j]
        buckets[index][4] += config3[j]
        buckets[index][5] += config4[j]
        buckets[index][6] += config5[j]
        buckets[index][7] += tabu[j]
        buckets[index][8] += 1

indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
buckets = [[x/y[-1] for x in y[:-1]] if y[-1] != 0 else [0., 0., 0., 0., 0., 0., 0., 0., 0] for y in buckets]
buckets = [[z / max(y) if max(y) != 0 else 0. for z in y] for y in buckets]

plt.figure()
plt.plot(indices, buckets)
plt.ylabel("Average performance")
plt.xlabel("Average Degree")
plt.legend(["greedy", "growth", "config1", "config2", "config3", "config4", "config5", "tabu-search"])
plt.savefig("plots/config_compare.png")
