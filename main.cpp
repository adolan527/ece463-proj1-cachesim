#include <iostream>
#include "Cache.h"
#include "Manager.h"
#include <random>



constexpr int argCount = 9;
struct CLI_Args{
    uint32_t BLOCKSIZE, L1_SIZE, L1_ASSOC, L2_SIZE, L2_ASSOC, PREF_N, PREF_M;
    char *trace_file;
};

// Total mem access should equal LLC read miss + write miss + writeback. In my case it is read miss + writeback. Therefore I need to fix write implementation or memory counting. (probabl the latter)

int main(int argc, char **argv) {


    bool debug_mode = false;
    CLI_Args args;

    if(argc!=argCount){
        if(argc==2 && argv[1][0]=='d'){
            debug_mode = true;
            printf("Running in debug mode\n");
        } else{
            printf("Usage: sim BLOCKSIZE L1_SIZE L1_ASSOC L2_SIZE L2_ASSOC PREF_N PREF_M trace_file");
            return 1;
        }
    }
    auto man = CacheSim::Manager();

if(!debug_mode) {
    args.BLOCKSIZE = atoll(argv[1]);
    args.L1_SIZE = atoll(argv[2]);
    args.L1_ASSOC = atoll(argv[3]);
    args.L2_SIZE = atoll(argv[4]);
    args.L2_ASSOC = atoll(argv[5]);
    args.PREF_N = atoll(argv[6]);
    args.PREF_M = atoll(argv[7]);
    args.trace_file = argv[8];
} else {
    args.BLOCKSIZE = 16;
    args.L1_SIZE = 1024;
    args.L1_ASSOC = 1;
    args.L2_SIZE = 8192;
    args.L2_ASSOC = 4;
    args.PREF_N = 0;
    args.PREF_M = 0;
    char *trace = "../example_trace.txt";
    args.trace_file = trace;
}

    man.AppendLayer("L1",args.L1_SIZE,args.L1_ASSOC,args.BLOCKSIZE);

    if(args.L2_SIZE != 0){
        man.AppendLayer("L2",args.L2_SIZE,args.L2_ASSOC,args.BLOCKSIZE);
    }

    man.AppendLayer("Memory",CacheSim::CacheType::Memory);

    auto file = fopen(args.trace_file,"r");
    if(!file){
        printf("Error: could not open file %s\n",args.trace_file);
        return 1;
    }
    man.ReadTrace(file);
    fclose(file);


    printf("\n");
    printf("===== Simulator configuration =====\n");
    printf("BLOCKSIZE:\t%d\n",args.BLOCKSIZE);
    printf("L1_SIZE:\t%d\n",args.L1_SIZE);
    printf("L1_ASSOC:\t%d\n",args.L1_ASSOC);
    printf("L2_SIZE:\t%d\n",args.L2_SIZE);
    printf("L2_ASSOC:\t%d\n",args.L2_ASSOC);
    printf("PREF_N:\t%d\n",args.PREF_N);
    printf("PREF_M:\t%d\n",args.PREF_M);
    printf("trace_file:\t%s\n",args.trace_file);
    printf("\n");
    man.PrintContents();
    man.PrintStatistics();

    //man.Statistics();
    return 0;


}

// 3 C's of misses
// Compulsory misses: First reference to a memory block
// Capacity and Conflict miss
// Scenario: memory is in cache, then replaced, then re-referenced and missed
// Capacity miss - Miss due to a limited capacity in the cache.
// Conflict - Miss within a set
// A  fully associative cache only suffers compulsory and capacity misses