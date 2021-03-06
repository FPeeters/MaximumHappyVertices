import subprocess
import csv
import os
import time
import numpy as np
import random
from multiprocessing import Pool, Lock


# Adapted from https://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
def print_progress(iteration, total, avg_time=0):
    if avg_time < 0:
        avg_time = 0
    filledLength = int(50 * iteration // total)
    bar = '█' * filledLength + '-' * (50 - filledLength)
    avg_time_str = str(round(avg_time)) + "s"
    time_remaining = '{0:02.0f}:{1:02.0f}'.format(*divmod(avg_time * (total - iteration) / threads, 60))
    print('\r|%s| %s avg: %s eta: %s ' % (bar, iteration, avg_time_str, time_remaining), end="")
    if iteration == total:
        print()


def run_instance(filename, method, nbNodes, nbColors, preColor, degree, alpha, seed):
    t = time.time()
    try:
        if method == "random":
            gen_result = subprocess.run(
                [exe_dir + os.path.sep + "generator", "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                 "-R", str(degree / (nbNodes - 1.)), "-s", str(seed), "-f", filename],
                stdout=subprocess.PIPE, universal_newlines=True)
            gen_result.check_returncode()
            gen = gen_result.stdout.split("\t")
            gen = [gen[1], gen[2], float(gen[3]) / nbNodes, gen[7], float(gen[6]) * (nbNodes - 1), alpha]
        elif method == "linear":
            filename = "../linearGraphs/graph" + str(nbNodes) + "_" + str(nbColors) + "_" + \
                       str(preColor) + "_" + str(degree) + "_" + str(alpha) + "_" + str(seed) + ".txt"
            gen = [nbNodes, nbColors, preColor, seed, degree, alpha]
        else:
            gen_result = subprocess.run(
                [exe_dir + os.path.sep + "generator", "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                 "-S", str(degree), "-s", str(seed), "-f", filename],
                stdout=subprocess.PIPE, universal_newlines=True)
            gen_result.check_returncode()
            gen = gen_result.stdout.split("\t")
            gen = [gen[1], gen[2], float(gen[3]) / nbNodes, gen[7], float(gen[6]) * (nbNodes - 1), alpha]

        stats = []

        for alpha in [-2., -1.5, -1., -0.5, 0, 0.5, 1., 1.5, 2]:
            happy = np.ndarray((SAMPLES,))
            for i in range(SAMPLES):
                result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "growth",
                                         "-red", "none", "-selectRandom", "-alpha", str(alpha),
                                         "-r", str(random.randint(0, 2 ** 8))],
                                        stdout=subprocess.PIPE, universal_newlines=True)
                happy[i] = int(result.stdout.split("\n")[-1])
                if alpha == -2. or alpha == 2.:
                    break
            if alpha == -2. or alpha == 2.:
                stats += [str(happy[0]), "0.0", str(happy[0])]
            else:
                stats += [str(np.average(happy)), str(np.std(happy)), str(np.max(happy))]

        result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "growth",
                                 "-red", "none"],
                                stdout=subprocess.PIPE, universal_newlines=True)

        lewis = int(result.stdout.split("\n")[-1])

        if method != "linear":
            os.remove(filename)
        return [method] + gen + [lewis] + stats, time.time() - t
    except Exception as e:
        print(filename, e)
        return [], time.time() - t


def callback(result):
    global avgTime, count, lock
    lock.acquire()
    if avgTime == -1:
        avgTime = result[1]
    else:
        avgTime = result[1] * emaFactor + avgTime * (1 - emaFactor)
    writer.writerow(result[0])
    count += 1
    print_progress(count, nbGraphs, avgTime)
    lock.release()


count = 0
avgTime = -1
emaFactor = 0

threads = 4
lock = Lock()
SAMPLES = 20

exe_dir = "../cmake-build-visual-studio"
# exe_dir = "../cmake-build"

if __name__ == '__main__':
    file = open("results.txt", "w", newline="")
    writer = csv.writer(file)

    nbNodes_options = [1000]
    nbColor_options = [10, 50]
    preColor_options = [0.05, 0.15, 0.25]

    degree_options = range(1, 26)
    alpha_options = [-2., -1.5, -1., -0.5, 0, 0.5, 1., 1.5, 2]
    scale_options = range(1, 13)

    seed_options = [591, 8412, 2107, 959, 3521, 9125, 2276, 2568, 2543, 29, 7131, 564, 2502, 4295, 5309, 2750, 319,
                    7074, 2605, 9193]

    nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * (
            len(degree_options) * (1 + len(alpha_options)) + len(scale_options)) * len(seed_options)
    emaFactor = 2 / (nbGraphs / len(nbNodes_options) + 1)

    print("Total graphs:", nbGraphs)
    print_progress(0, nbGraphs)

    pool = Pool(threads)

    fileCount = 0
    for nbNodes in nbNodes_options:
        for nbColors in nbColor_options:
            for preColor in preColor_options:
                if preColor * nbNodes < nbColors:
                    count += 1
                    print_progress(count, nbGraphs, avgTime)
                    continue

                for seed in seed_options:
                    for degree in degree_options:
                        fileCount += 1
                        filename = "../todo/graph" + str(fileCount) + ".txt"
                        pool.apply_async(run_instance,
                                         (filename, "random", nbNodes, nbColors, preColor, degree, 0, seed),
                                         callback=callback)

                        for alpha in alpha_options:
                            fileCount += 1
                            filename = "../todo/graph" + str(fileCount) + ".txt"
                            pool.apply_async(run_instance,
                                             (filename, "linear", nbNodes, nbColors, preColor, degree, alpha, seed),
                                             callback=callback)

                    for scale in scale_options:
                        fileCount += 1
                        filename = "../todo/graph" + str(fileCount) + ".txt"
                        pool.apply_async(run_instance,
                                         (filename, "scale", nbNodes, nbColors, preColor, scale, 0, seed),
                                         callback=callback)

    pool.close()
    pool.join()
    file.close()
