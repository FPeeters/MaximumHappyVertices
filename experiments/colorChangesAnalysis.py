import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("colorChangesResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha",
                          "simAnn", "simAnnTime", "tabu", "tabuTime"])

data = data.replace("No nodes left after reducing", "0")
data["simAnn"] = data["simAnn"].astype(int)

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

print("Average")
print(data[["simAnn", "simAnnTime", "tabu", "tabuTime"]].mean())
print("\nAverage ifv method")
print(data.groupby(["method"]).mean()[["simAnn", "simAnnTime", "tabu", "tabuTime"]])
print("\nAverage ifv nbColors, precolor")
print(data.groupby(["nbColors", "precolor"]).mean()[["simAnn", "simAnnTime", "tabu", "tabuTime"]])

data.groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["simAnn", "tabu"],
                                         label=["Sim. Ann.", "Tabu"])
plt.ylabel("# Kleurveranderingen")
plt.ticklabel_format(axis="y", style="sci", scilimits=(3, 3))
plt.savefig("colorchanges.png")

data.groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["simAnnTime", "tabuTime"],
                                         label=["Sim. Ann.", "Tabu"])
plt.ylabel("Uitvoeringstijd (s)")
plt.savefig("colorchanges_time.png")
