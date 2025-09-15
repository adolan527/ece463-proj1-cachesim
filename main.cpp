#include <iostream>
#include "Cache.h"
#include "Manager.h"
#include <random>

// Use this for submission
#define CLI_MODE true

constexpr int argCount = 9;
struct CLI_Args{
    uint32_t BLOCKSIZE, L1_SIZE, L1_ASSOC, L2_SIZE, L2_ASSOC, PREF_N, PREF_M;
    char *trace_file;
};

// Total mem access should equal LLC read miss + write miss + writeback. In my case it is read miss + writeback. Therefore I need to fix write implementation or memory counting. (probabl the latter)

int main(int argc, char **argv) {

uint32_t x = 0xFFFF0000;
printf("%x\n%x\n",x,x>>16);


#if CLI_MODE

    CLI_Args args;
    argc = argCount;
    if(argc!=argCount){
        perror("Usage: sim BLOCKSIZE L1_SIZE L1_ASSOC L2_SIZE L2_ASSOC PREF_N PREF_M trace_file");
        return 1;
    }
    args.BLOCKSIZE = atoll(argv[1]);
    args.L1_SIZE = atoll(argv[2]);
    args.L1_ASSOC = atoll(argv[3]);
    args.L2_SIZE = atoll(argv[4]);
    args.L2_ASSOC = atoll(argv[5]);
    args.PREF_N = atoll(argv[6]);
    args.PREF_M = atoll(argv[7]);
    args.trace_file = argv[8];

/*
 * Override for testing
    args.BLOCKSIZE = 16;
    args.L1_SIZE = 1024;
    args.L1_ASSOC = 1;
    args.L2_SIZE = 8192;
    args.L2_ASSOC = 4;
    args.PREF_N = 0;
    args.PREF_M = 0;
    char* trace = "../example_trace.txt";
    args.trace_file = trace;
*/
    auto man = CacheSim::Manager();

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

    man.Statistics();
    return 0;
#endif

#if !CLI_MODE

    auto man = CacheSim::Manager();

    man.AppendLayer("L1",2048,4,8);
    man.AppendLayer("L2",8192,16,8);
    man.AppendLayer("Memory",CacheSim::CacheType::Memory);

    man.StartDebugLayers();

    auto file = fopen("../example_trace.txt","r");
    if(!file){
        perror("Failed to open '../example_trace.txt'\n");
        return 1;
    }
    man.ReadTrace(file);
    fclose(file);


    //man.StopDebugLayers();
    file = fopen("../output.csv","w");
    if(!file){
        perror("Failed to open '../output.csv'\n");
        return 1;
    }
    man.PrintResults(file);
    man.Statistics();

    fclose(file);
    return 0;
#endif
}

// 3 C's of misses
// Compulsory misses: First reference to a memory block
// Capacity and Conflict miss
// Scenario: memory is in cache, then replaced, then re-referenced and missed
// Capacity miss - Miss due to a limited capacity in the cache.
// Conflict - Miss within a set
// A  fully associative cache only suffers compulsory and capacity misses