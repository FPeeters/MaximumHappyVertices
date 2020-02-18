import matplotlib.pyplot as plt

file = open("algorithmComparison.txt", "r")
data = [list(map(lambda x: float(x), line.split(','))) for line in file.readlines()]

# n=500
dens = [x[5] for x in data if x[0] == 500]
diff = [x[8] - max(x[6], x[7]) for x in data if x[0] == 500]
rel_diff = [(x[8] - max(x[6], x[7])) / max(x[6], x[7]) if max(x[6], x[7]) > 0 else 0 for x in data if x[0] == 500]

plt.subplot(2, 2, 1)
plt.title("n = 500")
plt.axis([0, max(dens), 0, max(diff) + 5])
plt.scatter(dens, diff, marker='.')

# n=1000
dens = [x[5] for x in data if x[0] == 1000]
diff = [x[8] - max(x[6], x[7]) for x in data if x[0] == 1000]
rel_diff = [(x[8] - max(x[6], x[7])) / max(x[6], x[7]) if max(x[6], x[7]) > 0 else 0 for x in data if x[0] == 1000]

plt.subplot(2, 2, 2)
plt.title("n = 1000")
plt.axis([0, max(dens), 0, max(diff) + 5])
plt.scatter(dens, diff, marker='.')

# n=1500
dens = [x[5] for x in data if x[0] == 1500]
diff = [x[8] - max(x[6], x[7]) for x in data if x[0] == 1500]
rel_diff = [(x[8] - max(x[6], x[7])) / max(x[6], x[7]) if max(x[6], x[7]) > 0 else 0 for x in data if x[0] == 1500]

plt.subplot(2, 2, 3)
plt.title("n = 1500")
plt.axis([0, max(dens), 0, max(diff) + 5])
plt.scatter(dens, diff, marker='.')

# n=2000
dens = [x[5] for x in data if x[0] == 2000]
diff = [x[8] - max(x[6], x[7]) for x in data if x[0] == 2000]
rel_diff = [(x[8] - max(x[6], x[7])) / max(x[6], x[7]) if max(x[6], x[7]) > 0 else 0 for x in data if x[0] == 2000]

plt.subplot(2, 2, 4)
plt.title("n = 2000")
plt.axis([0, max(dens), 0, max(diff) + 5])
plt.scatter(dens, diff, marker='.')

plt.show()
