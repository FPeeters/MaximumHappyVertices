import matplotlib.pyplot as plt
import math

file = open("tabuCompare.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]

x = [r[5] for r in results]
greedy = [r[6] for r in results]
growth = [r[7] for r in results]
sim = [r[8] for r in results]
tabu = [r[9] for r in results]

nbBuckets = 20
buckets = [[0., 0., 0., 0., 0] for _ in range(nbBuckets)]
for j in range(len(x)):
    index = math.floor((x[j] - min(x)) / max(x) * nbBuckets)
    if index == nbBuckets:
        buckets[-1][0] += greedy[j]
        buckets[-1][1] += growth[j]
        buckets[-1][2] += sim[j]
        buckets[-1][3] += tabu[j]
        buckets[-1][4] += 1
    else:
        buckets[index][0] += greedy[j]
        buckets[index][1] += growth[j]
        buckets[index][2] += sim[j]
        buckets[index][3] += tabu[j]
        buckets[index][4] += 1

indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
buckets = [[y[0] / y[4], y[1] / y[4], y[2] / y[4], y[3] / y[4]] if y[4] != 0 else [0., 0., 0., 0.] for y in buckets]
buckets = [[z / max(y) if max(y) != 0 else 0. for z in y] for y in buckets]

plt.plot(indices, buckets)
plt.ylabel("Average performance")
plt.xlabel("Denisty")
plt.legend(["greedy", "growth", "simulated annealing", "tabu-search"])
plt.savefig("plots/tabu_compare")
