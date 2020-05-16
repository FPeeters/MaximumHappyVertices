import subprocess
import os
import time
from multiprocessing import Pool, Lock


def generate(nbNodes, nbColors, preColor, degree, alpha, seed):
    t = time.time()
    filename = "clusterGraphs/graph" + str(nbNodes) + "_" + str(nbColors) + "_" + \
               str(preColor) + "_" + str(degree) + "_" + str(alpha) + "_" + str(seed) + ".txt"
    gen_result = subprocess.run([exe_dir + os.path.sep + "clusteringGenerator", filename,
                                 str(nbNodes), str(degree), str(alpha), str(nbColors),
                                 str(preColor), str(seed)])
    if gen_result.returncode != 0:
        print(filename, "failed")
    return time.time() - t


def callback(result):
    global i, lock
    lock.acquire()
    print("\r" + str(i) + "\t" + str(result), end="")
    i += 1
    lock.release()


i = 1
threads = 7
lock = Lock()

exe_dir = "cmake-build-visual-studio"
# exe_dir = "cmake-build"

if __name__ == '__main__':
    nbNodes_options = [1000]
    nbColor_options = [10, 50]
    preColor_options = [0.05, 0.15, 0.25]

    degree_options = range(0, 26)
    alpha_options = [-2., -1.5, -1., -0.5, 0, 0.5, 1., 1.5, 2]

    seed_options = [591, 8412, 2107, 959, 3521, 9125, 2276, 2568, 2543, 29, 7131, 564, 2502, 4295, 5309, 2750, 319,
                    7074, 2605, 9193]

    pool = Pool(threads)

    nbGraphs = len(nbNodes_options) * len(nbColor_options) * len(preColor_options) * (
            len(degree_options) * len(alpha_options)) * len(seed_options)

    print("Total graphs:", nbGraphs)

    for nbNodes in nbNodes_options:
        for nbColors in nbColor_options:
            for preColor in preColor_options:
                if preColor * nbNodes < nbColors:
                    continue
                for seed in seed_options:
                    for degree in degree_options:
                        for alpha in alpha_options:
                            pool.apply_async(generate, (nbNodes, nbColors, preColor, degree * 2, alpha, seed),
                                             callback=callback)

    pool.close()
    pool.join()
