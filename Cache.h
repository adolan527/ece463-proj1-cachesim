//
// Created by Aweso on 9/8/2025.
//

#ifndef ECE463_CACHE_H
#define ECE463_CACHE_H

#include "types.h"
#include "Set.h"

namespace CacheSim {
    class Cache  {
        // Cache parameters
        uint32_t m_size, m_assoc, m_blockSize;
        BitWidths m_bitWidths;

        // Next layer this makes calls to (psuedo linked list)
        Cache* m_nextLayer;

        std::string m_name;

        // Sets for simulation
        std::vector<Set> m_sets;

        // Special type for memory
        CacheType m_type;

        // Callback vector for writing debug info
        std::vector<DebugInfo> *m_results;

        CacheStats m_stats;



    public:
        Cache(const std::string &name, uint32_t SIZE, uint32_t ASSOC, uint32_t BLOCKSIZE, CacheType type = CacheType::Normal);

        Cache(const std::string& name, CacheType type);// for memory

        CacheResponse SendRequest(const CacheRequest &req);

        void SetNextLayer(Cache* next);

        void SetDebugVector(std::vector<DebugInfo> *results);

        void ResetDebugVector();

        CacheStats GetStats();

        std::string& GetName();

        CacheType GetType();

        void PrintContents(FILE *file);

    private:
        SetRequest CacheToSetRequest(const CacheRequest &req) const;


    };



}

#endif //ECE463_CACHE_H
