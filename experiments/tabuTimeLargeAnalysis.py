import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("tabuTimeLargeResults.txt", header=None,
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

divmean = binnedmean[["greedy", "growth", "simAnn", "tabu"]].div(
    binnedmean[["greedy", "growth", "simAnn", "tabu"]].max(axis=1), axis=0)
divmean["Gemiddelde graad"] = binnedmean["Gemiddelde graad"]
divmean.plot(x="Gemiddelde graad", y=["greedy", "growth", "simAnn", "tabu"],
             label=["Greedy", "Growth", "Sim. Annealing", "Tabu-search"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_large_vgl_growth.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn", "tabu"],
             label=["Greedy", "Sim. Annealing", "Tabu-search"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_large_vgl.png")
