import matplotlib.pyplot as plt
import math

file = open("exactResults.txt", "r")
results = [list(map(lambda x: float(x) if x != "\n" else 100., line.split(','))) for line in file.readlines() if
           len(line.split(',')) != 4]
file.close()

diff = [x[23] - x[27] for x in results]

# colorFilter = [1 if 0.002 < x[2] < 0.013 else 0 for x in features]

feature_names = ["Number of nodes", "Number of edges", "Density", "Average degree", "Std of degree",
                 "Average shortest path length", "Diameter", "Minimum cylce length", "Average centrality",
                 "Std of centrality", "Average clustering", "Wiener index", "Average absolute eigenvalue",
                 "Std of eienvalues", "Algebraic connectivity", "Average eigenvector centrality",
                 "Std of eigenvector centrality", "Number of Colors", "Proportion of precolored vertices"]
for i in range(len(feature_names)):
    plt.figure()
    plt.title(feature_names[i])
    x = [y[i + 4] for y in results]
    if not math.isfinite(max(x)):
        x = [0.5 if math.isfinite(y) else 1.5 for y in x]
        plt.axis([0, 2, 0, max(diff) + 5])
        plt.xticks([0.5, 1.5], ['real', 'inf'])
    else:
        plt.axis([0, max(x), 0, max(diff) + 5])
    plt.scatter(x, diff, marker='.')
    plt.savefig("plots/metaExact_" + feature_names[i] + ".png")
    plt.close()
