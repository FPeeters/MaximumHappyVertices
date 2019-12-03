import glob
import sys
import subprocess
import time
from multiprocessing.pool import ThreadPool

cmdArgs = sys.argv[1:] + ["-threads", "6", "-time", "360"]


def work(filename):
    print("Starting ", filename)
    t = time.time()
    result = subprocess.run(["cmake-build-visual-studio\\main.exe"] + [filename] + cmdArgs,
                            stdout=subprocess.PIPE, universal_newlines=True)
    t = time.time() - t

    f = open(filename + ".log", "w")
    f.write(result.stdout)
    f.close()

    print(filename, " done")
    if result.returncode != 0:
        return -1, t

    happy = result.stdout.split("\n")[-1]
    return happy, t


# tp = ThreadPool(6)
results = []

for file in glob.glob("todo/*.txt"):
    results.append((file, work(file)))

#     results.append((file, tp.apply_async(work, [file])))
#
# tp.close()
# tp.join()
#
# results = [(f.split('\\')[-1], r.get()) for (f, r) in results]

print(results)
