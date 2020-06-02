import matplotlib.pyplot as plt
import math

file = open("growthSeedResults4.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 0., line.split(','))) for line in file.readlines()]
file.close()

nbBuckets = 30

# results = [x[:8] + x[9:] for x in results if x[5] * (x[0] - 1.)]

x = [y[5] * (y[0] - 1.) for y in results]
lewis = [y[6] for y in results]
averages = dict()
stds = dict()
bests = dict()

i = 0
for alpha in [-2., -1.8, -1.1, -0.75, -0.08, 1., 1.5, 1.7, 1.9, 2.]:
    averages[alpha] = [y[7 + i * 3] for y in results]
    stds[alpha] = [y[8 + i * 3] for y in results]
    bests[alpha] = [y[9 + i * 3] for y in results]
    i += 1

plt.figure()

for alpha in [-2., -1.8, -1.1, -0.75, -0.08, 1., 1.5, 1.7, 1.9, 2.]:
    buckets = [[0., 0., 0., 0., 0] for _ in range(nbBuckets)]
    for j in range(len(x)):
        index = math.floor((x[j] - min(x)) / max(x) * nbBuckets)
        if index == nbBuckets:
            buckets[-1][0] += averages[alpha][j]
            buckets[-1][1] += bests[alpha][j]
            buckets[-1][2] += stds[alpha][j]
            buckets[-1][3] += lewis[j]
            buckets[-1][4] += 1
        else:
            buckets[index][0] += averages[alpha][j]
            buckets[index][1] += bests[alpha][j]
            buckets[index][2] += stds[alpha][j]
            buckets[index][3] += lewis[j]
            buckets[index][4] += 1

    indices = [min(x) + i * (max(x) - min(x)) / nbBuckets for i in range(nbBuckets)]
    buckets = [[y[0] / y[4], y[1] / y[4], y[2] / y[4], y[3] / y[4]] if y[4] != 0 else [0., 0., 0., 0.] for y in buckets]
    # if alpha in [-2., -1.9, -1.5, -1.]:
    plt.errorbar(indices, [y[0] for y in buckets], yerr=[y[2] for y in buckets], label=str(alpha))
        # plt.plot(indices, [y[1] for y in buckets], label=str(alpha))
plt.plot(indices, [y[3] for y in buckets], label="Lewis")

plt.title("Best results")
plt.legend()
plt.ylabel("nbHappy")
plt.xlabel("Average degree")
plt.savefig("plots/growth_seed4.png")
plt.close()
