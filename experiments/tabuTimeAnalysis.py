import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("tabuTimeResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha",
                          "greedy", "growth", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"])

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

binnedmean = data.groupby("avgDegreeCut").mean()

divmean = binnedmean[["greedy", "growth", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"]].div(
    binnedmean[["greedy", "growth", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"]].max(axis=1), axis=0)
divmean["Gemiddelde graad"] = binnedmean["Gemiddelde graad"]

divmean.plot(x="Gemiddelde graad", y=["greedy", "growth", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"],
             label=["greedy", "growth", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_growth.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"],
             label=["greedy", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10"],
             label=["greedy", "simAnn1", "simAnn2", "simAnn4", "simAnn7", "simAnn10"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_sim.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"],
             label=["greedy", "tabu1", "tabu2", "tabu4", "tabu7", "tabu10"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_tabu.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn1", "tabu1"],
             label=["greedy", "simAnn1", "tabu1"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_1.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn2", "tabu2"],
             label=["greedy", "simAnn2", "tabu2"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_2.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn4", "tabu4"],
             label=["greedy", "simAnn4", "tabu4"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_4.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn7", "tabu7"],
             label=["greedy", "simAnn7", "tabu7"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_7.png")

divmean.plot(x="Gemiddelde graad", y=["greedy", "simAnn10", "tabu10"],
             label=["greedy", "simAnn10", "tabu10"])
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("tabu_vgl_time_10.png")
