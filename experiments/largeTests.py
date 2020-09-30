import subprocess
import glob
import os
import time

exe_dir = "../cmake-build-visual-studio"
# exe_dir = "../cmake-build"

for filename in glob.glob("../largeGraphs/largeScale*"):
    file = open("largeResults2.txt", "a")
    greedy_result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "greedy",
                                    "-red", "articul"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    greedy_result.check_returncode()
    file.write("++++" * 10 + "\n")
    file.write(filename + " -- GREEDY\n")
    file.write(greedy_result.stdout)
    file.write("\n")
    file.write(greedy_result.stderr)
    file.write("\n")
    print(filename + " Greedy done")

    growth_result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "growth",
                                    "-red", "articul"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    growth_result.check_returncode()
    file.write("----" * 10 + "\n")
    file.write(filename + " -- GROWTH\n")
    file.write(growth_result.stdout)
    file.write("\n")
    file.write(growth_result.stderr)
    file.write("\n")
    print(filename + " Growth done")

    sim_result = subprocess.run([exe_dir + os.path.sep + "main", filename, "-a", "simAnn", "-red", "articul",
                                 "-init", "best", "-time", "1800", "-temp", "163", "-zeroTemp", "0.05",
                                 "-swap", "0.5", "-split", "0.25"],
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    sim_result.check_returncode()
    file.write("----" * 10 + "\n")
    file.write(filename + " -- SIMANN\n")
    file.write(sim_result.stdout)
    file.write("\n")
    file.write(sim_result.stderr)
    file.write("\n")
    print(filename + " SimAnn done")

    t = time.time()
    tabu_result = subprocess.run([exe_dir + os.path.sep + "happyTabu", filename, "-t", "300"],
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    t = time.time() - t
    file.write("----" * 10 + "\n")
    file.write(filename + " -- TABU\n")
    file.write(tabu_result.stdout)
    file.write("\n")
    file.write(tabu_result.stderr)
    file.write("Took " + str(t) + " seconds")
    file.write("\n")
    print(filename + " Tabu done")

    file.close()
