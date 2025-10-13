
import os
import sys
import subprocess
import csv

def runCacheSim( BLOCKSIZE, size, assoc):
    return subprocess.run(
        ["..\\cmake-build-debug\\ece463.exe", str(BLOCKSIZE), str(size), str(assoc), "0", "0", "0", "0", "..\\traces\\gcc_trace.txt"],
        stdout=subprocess.PIPE,
        text=True
    )

def getField(result,field_tag):
    for line in result.stdout.splitlines():
        if field_tag in line:
            return line.split(':')[1]

def main():


    with open("..\\test_results\\results_4.csv", "w", newline="") as f:
        writer = csv.writer(f)
        blocksizes = [16, 32, 64, 128]
        writer.writerow(["Size","16","32","64","128"])
        assoc = 4
        for s in range(0,7):
            size = 1024 * (2**s)
            print(f"Size: {size}")
            temp = [size]
            for b in blocksizes:
                print(f"BS: {b}")
                mr = getField(runCacheSim(b,size,assoc),"e. L1 miss rate:")
                temp.append(mr)
            writer.writerow(temp)




if __name__ == "__main__":
    main()