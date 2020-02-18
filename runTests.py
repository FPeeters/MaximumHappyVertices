import glob
import sys
import subprocess
import time
import csv

cmdArgs = sys.argv[1:] + ["-a", "simAnn", "-init", "growth", "-temp", "93", "-swap", "0", "-split", "0.27",
                          "-threads", "6", "-time", "900"]


def work(filename):
    print("Starting ", filename)
    t = time.time()
    result = subprocess.run(["cmake-build-visual-studio\\main.exe"] + [filename] + cmdArgs,
                            stdout=subprocess.PIPE, universal_newlines=True)
    t = time.time() - t

    print(filename, " done")
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)
        return -1, t

    lines = result.stdout.split("\n")
    # gap = lines[-2]
    happy = lines[-1]
    return happy, t, # gap


results = []

for file in glob.glob("testInstances/*.txt"):
    results.append((file.split("\\")[-1], work(file)))

file = open("results.txt", "w")
writer = csv.writer(file)

for res in results:
    writer.writerow([res[0]] + list(res[1]))

file.close()
