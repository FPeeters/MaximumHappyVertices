import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("growthResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha", "Lewis",
                          "-2mean", "-2std", "-2best", "-1.5mean", "-1.5std", "-1.5best", "-1mean", "-1std", "-1best",
                          "-.5mean", "-.5std", "-.5best", "0mean", "0std", "0best", ".5mean", ".5std", ".5best",
                          "1mean", "1std", "1best", "1.5mean", "1.5std", "1.5best", "2mean", "2std", "2best"])

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

binnedmean = data.groupby("avgDegreeCut").mean()

divmean = binnedmean[["Lewis", "-2mean", "-1.5mean", "-1mean", "-.5mean", "0mean",
                      ".5mean", "1mean", "1.5mean", "2mean"]].div(
    binnedmean[["Lewis", "-2mean", "-1.5mean", "-1mean", "-.5mean", "0mean",
                ".5mean", "1mean", "1.5mean", "2mean"]].max(axis=1), axis=0)
divmean["Gemiddelde graad"] = binnedmean["Gemiddelde graad"]

fig = plt.figure(figsize=[9.6, 7.2])
divmean.plot(x="Gemiddelde graad",
             y=["Lewis", "-2mean", "-1.5mean", "-1mean", "-.5mean", "0mean", ".5mean", "1mean", "1.5mean", "2mean"],
             label=["Lewis", "alfa=-2", "alfa=-1.5", "alfa=-1", "alfa=-0.5",
                    "alfa=0", "alfa=0.5", "alfa=1", "alfa=1.5", "alfa=2"], ax=plt.gca())
plt.ylabel("Rel. gemiddeld # Happy knopen")
plt.savefig("growth_relative.png")

print("Total: ")
print("Lewis < -1.5 best")
print((data["Lewis"] < data["-1.5best"]).value_counts())
print("\nLewis < -1.5 mean")
print((data["Lewis"] < data["-1.5mean"]).value_counts())
