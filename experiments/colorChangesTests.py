import subprocess
import csv
import os
import time
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

        sim_time = time.time()
        sim_result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "simAnn", "-red", "articul",
                                 "-init", "best", "-maxI", "5000", "-temp", "211", "-zeroTemp", "0.05",
                                 "-swap", "0.05", "-split", "0.43"],
                                stdout=subprocess.PIPE, universal_newlines=True)
        sim_time = time.time() - sim_time
        sim_result.check_returncode()
        simAnn = sim_result.stdout.split("\n")[-2].split(": ")[-1]

        tabu_time = time.time()
        tabu_result = subprocess.run([exe_dir + os.path.sep + "happyTabu", filename, "-t", "5"],
                                     stdout=subprocess.PIPE, universal_newlines=True)
        tabu_time = time.time() - tabu_time
        tabu = tabu_result.stdout.split("\n")[-2].split("\t")[-2]

        if method != "linear":
            os.remove(filename)
        return [method] + gen + [simAnn, sim_time, tabu, tabu_time], time.time() - t
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

threads = 7
lock = Lock()

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
    # 591, 8412, 2107, 959
    seed_options = [3521, 9125, 2276, 2568, 2543, 29, 7131, 564, 2502, 4295, 5309, 2750, 319,
                    7074, 2605, 9193]

    nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * (
            len(degree_options) * (1 + len(alpha_options)) + len(scale_options)) * len(seed_options)
    emaFactor = 2 / (nbGraphs / len(nbNodes_options) / len(seed_options) + 1)

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
