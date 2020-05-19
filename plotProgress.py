import matplotlib.pyplot as plt

file = open("progress.txt")
progress = [list(map(lambda x: float(x) if x != "\n" else 0., line.split('\t'))) for line in file.readlines()]

plt.plot([x[0] for x in progress], [x[1:] for x in progress])
plt.legend(["Temperature", "Energy", "Best Energy"])
plt.show()
