import subprocess
import os
import time

exe_dir = "cmake-build-visual-studio"
# exe_dir = "cmake-build"

nbNodes = 100000
nbColors = 50
preColor = 0.1
degree = 5
scale = 3

count = 0
# gen[6], density, overflows in generator so is incorrect
for seed in [591, 8412, 2107, 959, 3521, 9125, 2276, 2568, 2543, 29,
             7131, 564, 2502, 4295, 5309, 2750, 319, 7074, 2605, 9193]:
    count += 1
    t = time.time()
    gen_result = subprocess.run(
        [exe_dir + os.path.sep + "generator", "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
         "-R", str(degree / (nbNodes - 1.)), "-s", str(seed), "-f", "todo/largeRandom" + str(count) + ".txt"],
        stdout=subprocess.PIPE, universal_newlines=True)
    gen_result.check_returncode()
    gen = gen_result.stdout.split("\t")
    print(gen[0], gen[1], gen[2], float(gen[3]) / nbNodes, gen[7], float(gen[6]) * (nbNodes - 1))
    print("took", time.time() - t)

count = 0

for seed in [591, 8412, 2107, 959, 3521, 9125, 2276, 2568, 2543, 29,
             7131, 564, 2502, 4295, 5309, 2750, 319, 7074, 2605, 9193]:
    count += 1
    t = time.time()
    gen_result = subprocess.run(
        [exe_dir + os.path.sep + "generator", "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
         "-S", str(scale), "-s", str(seed), "-f", "todo/largeScale" + str(count) + ".txt"],
        stdout=subprocess.PIPE, universal_newlines=True)
    gen_result.check_returncode()
    gen = gen_result.stdout.split("\t")
    print(gen[0], gen[1], gen[2], float(gen[3]) / nbNodes, gen[7], float(gen[6]) * (nbNodes - 1))
    print("took", time.time() - t)
