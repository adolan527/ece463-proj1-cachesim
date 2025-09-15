//
// Created by Aweso on 9/8/2025.
//

#ifndef ECE463_SET_H
#define ECE463_SET_H
#include "types.h"
namespace CacheSim {



    class Set {
    protected:

        struct Block{
            uint32_t counter, tag; // a better datatype may exist, this is safe
            bool valid;
            bool dirty;
            Block():
                    valid(false), dirty(false), counter(0), tag(0){
            };

        };

        std::vector<Block> m_blocks;

        uint32_t m_blocksize;


    public:
        Set(uint32_t ways, uint32_t blocksize);

        SetResponse SendRequest(SetRequest req);


    private:
        void IncLessThan(uint32_t threshold); // Used internally for updating LRU counters

    };



} // CacheSim

#endif //ECE463_SET_H
