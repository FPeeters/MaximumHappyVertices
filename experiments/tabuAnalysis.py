import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("newTabuResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha",
                          "greedy", "growth", "simAnn", "tabu"])

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

data["better"] = data["tabu"] >= data["simAnn"]

print("Tabu beter dan simAnn")
print(data["better"].value_counts())
print("\nAnalyse cases simAnn better")
print(data["precolor"][~data["better"]].value_counts())
print("Gemiddelde graad")
print(data["Gemiddelde graad"][~data["better"]].mean())

binnedmean = data.groupby("avgDegreeCut").mean()

divmean = data[["greedy", "growth", "simAnn", "tabu"]].div(
    data[["greedy", "growth", "simAnn", "tabu"]].max(axis=1), axis=0)
divmean["avgDegreeCut"] = data["avgDegreeCut"]
divmean["Gemiddelde graad"] = data["Gemiddelde graad"]
divmean.groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["greedy", "growth", "simAnn", "tabu"],
             label=["Greedy", "Growth", "Sim. Annealing", "Tabu-search"], color='black')
plt.gca().get_lines()[0].set_linestyle('--')
plt.gca().get_lines()[1].set_linestyle('-')
plt.gca().get_lines()[2].set_linestyle('-.')
plt.gca().get_lines()[3].set_linestyle(':')
plt.legend()
plt.xlabel("Average degree")
plt.ylabel("Relative performance")
plt.savefig("tabu_vgl_growth.eps")
plt.savefig("tabu_vgl_growth_new.png")

divmean.groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["greedy", "simAnn", "tabu"],
             label=["Greedy", "Sim. Annealing", "Tabu-search"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_new.png")
