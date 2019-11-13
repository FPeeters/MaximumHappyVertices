import glob
import sys
import subprocess
from multiprocessing.pool import ThreadPool

cmdArgs = sys.argv[1:]

def work(filename):
    result = subprocess.run(["cmake-build-visual-studio\\main.exe"] + [filename] + cmdArgs, stdout=subprocess.PIPE, universal_newlines=True)

    print(filename, " done")
    if result.returncode != 0:
        return -1

    happy = result.stdout.split("\n")[-1]
    return happy


tp = ThreadPool(1)
results = []

for file in glob.glob("testInstances/*.txt"):
    results.append((file, tp.apply_async(work, [file])))

tp.close()
tp.join()
results = [(f.split('\\')[-1], r.get()) for (f, r) in results]

print(results)
