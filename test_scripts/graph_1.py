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


    with open("..\\test_results\\results_1.csv", "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Size (B)", "Associativity", "L1 Miss Rate"])
        BLOCKSIZE = 32
        for s in range(0,11):
            size = 1024 * (2**s)
            print(f"Size: {size}")
            assoc = 1
            while assoc < size // BLOCKSIZE and assoc <= 8:
                print(f"Associativity: {assoc}")
                miss_rate = getField(runCacheSim(BLOCKSIZE, size, assoc), "e. L1 miss rate:")
                writer.writerow([size, assoc, miss_rate])
                assoc *= 2

            # fully associative
            miss_rate = getField(runCacheSim(BLOCKSIZE, size, size // BLOCKSIZE), "e. L1 miss rate:")
            writer.writerow([size, size // BLOCKSIZE, miss_rate])













if __name__ == "__main__":
    main()