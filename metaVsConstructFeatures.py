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

diff = [x[8] - max(x[6], x[7]) for x in results]

colorFilter = [1 if 0.002 < x[2] < 0.013 else 0 for x in features]

feature_names = ["Number of nodes", "Number of edges", "Density", "Average degree", "Std of degree",
                 "Average shortest path length", "Diameter", "Minimum cylce length", "Average centrality",
                 "Std of centrality", "Average clustering", "Wiener index", "Average absolute eigenvalue",
                 "Std of eienvalues", "Algebraic connectivity", "Average eigenvector centrality",
                 "Std of eigenvector centrality", "Number of Colors", "Proportion of precolored vertices"]
for i in range(len(features[0])):
    plt.figure()
    plt.title(feature_names[i])
    x = [y[i] for y in features]
    if not math.isfinite(max(x)):
        x = [0.5 if math.isfinite(y) else 1.5 for y in x]
        plt.axis([0, 2, 0, max(diff) + 5])
        plt.xticks([0.5, 1.5], ['real', 'inf'])
    else:
        plt.axis([0, max(x), 0, max(diff) + 5])
    plt.scatter(x, diff, marker='.', c=colorFilter)
    plt.savefig("plots/construct_" + feature_names[i] + ".png")
    plt.close()
