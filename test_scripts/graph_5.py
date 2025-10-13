
import os
import sys
import subprocess
import csv

def runCacheSim(size1,size2):
    return subprocess.run(
        ["..\\cmake-build-debug\\ece463.exe", "32", str(size1), "4", str(size2), "8", "0", "0", "..\\traces\\gcc_trace.txt"],
        stdout=subprocess.PIPE,
        text=True
    )

def getField(result,field_tag):
    for line in result.stdout.splitlines():
        if field_tag in line:
            return line.split(':')[1]

def main():


    with open("..\\test_results\\results_5.csv", "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["L1 Size","16KB","32KB","64KB"])
        assoc = 4
        for s in range(0,4):
            size = 1024 * (2**s)
            print(f"Size: {size}")
            temp = [size]
            for l2s in range(14,17):
                l2size = 2**l2s
                mr = getField(runCacheSim(size,l2size),"e. L1 miss rate:")
                temp.append(mr)
            writer.writerow(temp)

        for s in range(0,4):
            size = 1024 * (2**s)
            print(f"Size: {size}")
            temp = [size]
            for l2s in range(14,17):
                l2size = 2**l2s
                mr = getField(runCacheSim(size,l2size),"n. L2 miss rate:  ")
                temp.append(mr)
            writer.writerow(temp)



if __name__ == "__main__":
    main()