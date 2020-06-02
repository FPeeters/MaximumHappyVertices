import subprocess

count = 0

for nbColors in [10, 50]:
    for precolor in [0.05, 0.15, 0.25]:
        for degree in range(1, 26):
            result = subprocess.run(["cmake-build-visual-studio\\generator.exe", "-n", "1000", "-k", str(nbColors),
                                     "-p", str(precolor), "-R", str(degree / 999.), "-f",
                                     "tuningInstances/random" + str(count) + ".txt", "-s", str(54321)])
            count += 1
