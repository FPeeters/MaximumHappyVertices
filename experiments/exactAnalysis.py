import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("exactResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha",
                          "first", "firstGap", "firstTime", "second", "secondGap", "secondTime"])

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

print("Solve time ifv method")
print(data.groupby("method").mean()[["firstTime", "secondTime"]])

print("\nSolve time ifv nbcolors, precolor")
print(data.groupby(["nbColors", "precolor"]).mean()[["firstTime", "secondTime"]])

data.groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["firstTime", "secondTime"],
                                         label=["Eerste model", "Tweede model"])
plt.ylabel("Tijd (s)")
plt.savefig("exact_time.png")
