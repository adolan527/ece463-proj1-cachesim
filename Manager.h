//
// Created by Aweso on 9/10/2025.
//

#ifndef ECE463_MANAGER_H
#define ECE463_MANAGER_H
#include "types.h"
#include "Cache.h"
namespace CacheSim {

    // Manager class to hold cache and memory
    class Manager {
        std::list<Cache*> m_layers; //pointers to layers (L1 - LLC - Mem)

        std::vector<DebugInfo> m_results;

    public:
        Manager(size_t layerCount = 4);

        ~Manager();

        void AppendLayer(const std::string &name, uint32_t SIZE, uint32_t ASSOC, uint32_t BLOCKSIZE, CacheType type = CacheType::Normal);

        void AppendLayer(const std::string& name, CacheType type);// for memory

        void StartDebugLayers();

        void StopDebugLayers();

        CacheResponse SendRequest(CacheRequest req);

        void ReadTrace(FILE *file,uint32_t lines = UINT32_MAX);

        void PrintResults(FILE *file=stdout);

        void PrintContents(FILE *file=stdout);

        void PrintStatistics(FILE *file=stdout);

    };

} // CacheSim

#endif //ECE463_MANAGER_H
