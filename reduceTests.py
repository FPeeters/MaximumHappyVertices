import subprocess
import csv
import os
import time
from multiprocessing import Pool, Lock

import clusteringGenerator


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


def run_instance(filename, nbNodes, nbColors, preColor, degree, cluster, seed):
    t = time.time()
    gen_result = subprocess.run(["cmake-build-visual-studio\\generator.exe",
                                 "-n", str(nbNodes), "-k", str(nbColors), "-p", str(preColor),
                                 "-R", str(degree / (nbNodes - 1.)), "-s", str(seed), "-f", filename],
                                stdout=subprocess.PIPE, universal_newlines=True)

    # graph = clusteringGenerator.generate_graph(seed, nbNodes, degree / (nbNodes - 1.), cluster, nbColors, preColor)
    # clusteringGenerator.write_to_file(filename, graph, nbNodes, degree / (nbNodes - 1.), nbColors)
    # gen = [nbNodes, nbColors, preColor, seed, degree, cluster]

    thiruv_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename, "-red", "thiruvady",
                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)
    basic_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename, "-red", "basic",
                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)
    articul_result = subprocess.run(["cmake-build-visual-studio\\main.exe", filename, "-red", "articul",
                                    "-a", "greedy"], stdout=subprocess.PIPE, universal_newlines=True)

    try:
        gen = gen_result.stdout.split("\t")
        gen[3] = str(float(gen[3]) / nbNodes)
        gen = gen[1:4] + gen[7:8] + gen[5:7]
        thiruv = str(sum(map(lambda x: int(x.split(" ")[0]), thiruv_result.stdout.split("\n")[1:4])))
        basic = str(sum(map(lambda x: int(x.split(" ")[0]), basic_result.stdout.split("\n")[1:5])))
        articul = str(sum(map(lambda x: int(x.split(" ")[0]), articul_result.stdout.split("\n")[2:6])))

        os.remove(filename)
        return gen + [thiruv, basic, articul], time.time() - t
    except:
        print("An error occured")
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
tunedArgs = ["-a", "simAnn", "-maxI", "5000", "-init", "best", "-temp", "51", "-swap", "0.05", "-split",
             "0.06"]  # "-time", "30"
threads = 4
lock = Lock()

if __name__ == '__main__':
    file = open("results.txt", "w", newline="")
    writer = csv.writer(file)

    nbNodes_options = [1000]
    nbColor_options = [5 , 10, 15, 20]
    preColor_options = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40]
    degree_options = range(0, 1000)
    cluster_options = [0]
    seed_options = [1234, 4321]

    nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * \
               len(degree_options) * len(cluster_options) * len(seed_options)
    emaFactor = 2 / (nbGraphs / len(nbNodes_options) + 1)

    print("Total graphs:", nbGraphs)
    print_progress(0, nbGraphs)

    pool = Pool(threads)

    fileCount = 0
    for nbNodes in nbNodes_options:
        for nbColors in nbColor_options:
            for preColor in preColor_options:
                for degree in degree_options:
                    for cluster in cluster_options:
                        for seed in seed_options:
                            fileCount += 1
                            if preColor * nbNodes < nbColors:
                                writer.writerow([nbNodes, nbColors, nbNodes * preColor, seed])
                                count += 1
                                print_progress(count, nbGraphs, avgTime)
                                continue

                            filename = "todo/graph" + str(fileCount) + ".txt"
                            pool.apply_async(run_instance,
                                             (filename, nbNodes, nbColors, preColor, degree, cluster, seed),
                                             callback=callback)

    pool.close()
    pool.join()
    file.close()
