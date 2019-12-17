import subprocess

count = 31

for links in [1, 3, 5, 7, 9, 11]:
    for precolor in [50, 100, 150, 200]:
        for seed in [123, 321]:
            result = subprocess.run(["cmake-build-visual-studio\\generator.exe", "-n", "1000", "-k", "10", "-p", str(precolor/1000), "-S", str(links), "-f", "todo/scale" + str(count) + ".txt", "-s", str(seed)])
            count += 1
            print(result.stdout)

