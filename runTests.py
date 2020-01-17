import glob
import sys
import subprocess
import time
import csv

cmdArgs = sys.argv[1:] + ["-a", "exact", "-time", "900", "-threads", "6"]


def work(filename):
    print("Starting ", filename)
    t = time.time()
    result = subprocess.run(["cmake-build-visual-studio\\main.exe"] + [filename] + cmdArgs,
                            stdout=subprocess.PIPE, universal_newlines=True)
    t = time.time() - t

    print(filename, " done")
    if result.returncode != 0:
        return -1, t

    lines = result.stdout.split("\n")
    gap = lines[-2]
    happy = lines[-1]
    return happy, t, gap


results = []

for file in glob.glob("todo/*.txt"):
    results.append((file.split("\\")[-1], work(file)))

file = open("results.txt", "w")
writer = csv.writer(file)

for res in results:
    writer.writerow([res[0]] + list(res[1]))

file.close()
