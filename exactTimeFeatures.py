import matplotlib.pyplot as plt
import math

file = open("exactResults2.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]
# results = [x for x in results if x[-1] != 0]
file.close()

nbBuckets = 50

for n in [50, 100, 150, 200]:
    x = [y[5] * (n-1) for y in results if y[0] == n]
    exactTime = [y[-1] for y in results if y[0] == n]

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

plt.legend(["n=50", "n=100", "n=150", "n=200", "n=500"])
plt.ylabel("Average execution time (ms)")
plt.xlabel("Density")
plt.semilogy()
plt.savefig("plots/exact_time.png")
plt.close()
