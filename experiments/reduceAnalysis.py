import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("reduceResults.txt", header=None,
                   names=["method", "n", "nbColors", "precolor", "seed", "Gemiddelde graad", "alpha", "thiruvady",
                          "basic", "articul"])

data["Eenvoudig"] = data.diff(axis=1)["basic"]
data["Articulatie"] = data.diff(axis=1, periods=2)["articul"]

data["avgDegreeCut"] = pd.cut(data["Gemiddelde graad"], 25, include_lowest=True)

random = data["method"] == "random"
scale = data["method"] == "scale"
linear = data["method"] == "linear"

print("Diff ifv methode, nbColors")
print(data.groupby(["method", "nbColors"]).mean().loc[:, ["Eenvoudig", "Articulatie"]])
print("\nDiff ifv graad voor scale")
print(data[scale].groupby("Gemiddelde graad").mean().loc[:, ["Eenvoudig", "Articulatie"]])
print("\nDiff ifv alpha voor linear")
print(data[linear].groupby("alpha").mean().loc[:, ["Eenvoudig", "Articulatie"]])
print("\nDiff ifv methode, precolor")
print(data.groupby(["method", "precolor"]).mean().loc[:, ["Eenvoudig", "Articulatie"]])
print("\nArticul diff ifv graad voor linear -2")
print(data[data["alpha"] == -2.].groupby("Gemiddelde graad").mean()["Articulatie"])

data[random].groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["Eenvoudig", "Articulatie"])
plt.ylabel("# knopen")
plt.savefig("reductie_random.png")

data[linear].groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y=["Eenvoudig", "Articulatie"])
plt.ylabel("# knopen")
plt.savefig("reductie_linear.png")

plt.figure()
data[random].groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y="articul", label="Random", ax=(plt.gca()))
data[linear].groupby("avgDegreeCut").mean().plot(x="Gemiddelde graad", y="articul", label="Lineair", ax=(plt.gca()))
scale_mean = data[scale].groupby("avgDegreeCut").mean()
scale_mean[scale_mean["articul"].notnull()].plot(x="Gemiddelde graad", y="articul", label="Scale-free", ax=(plt.gca()))
plt.ylabel("# knopen")
plt.savefig("reductie_articul.png")
