//
// Created by Aweso on 9/8/2025.
//

#include "Set.h"
#include <algorithm>

namespace CacheSim {

    Set::Set(uint32_t ways, uint32_t blocksize) :
    m_blocksize(blocksize) {
        m_blocks.resize(ways);
        for(uint32_t i = 0; i < ways; i++){
            m_blocks[i].counter = i;
        } // set counters to 0-MAX
    }

    SetResponse Set::SendRequest(SetRequest req) {
        SetResponse resp = {false,false,0};
        int index = 0;

        for(size_t i = 0; i <m_blocks.size() ;i++){ // check for tag matches
            if(m_blocks[i].valid && m_blocks[i].tag == req.tag){ // if the tag matches
                resp.hit = true; // hit
                index = i; // target block
                goto done; // no more iteration is necessary
            }

            index = m_blocks[i].counter > m_blocks[index].counter // else target block is LRU
                    ? i
                    : index;
        }


        // cleanup required regardless of previous execution
        done:
        if(!resp.hit) { //miss, eviction
            bool hasempty = false;
            for (int i = 0 ; i< m_blocks.size(); i++) {
                if (!m_blocks[i].valid && i != index && m_blocks[index].valid) {
                    printf("Evicting valid when invalid exists\n");
                }
            }
            if(m_blocks[index].valid && m_blocks[index].dirty) { //we are evicting a dirty block
                resp.dirty = true;
                resp.dirty_address = m_blocks[index].address;
                m_blocks[index].dirty = false;
            }
        }
        IncLessThan(m_blocks[index].counter);
        m_blocks[index].counter = 0;
        m_blocks[index].address = req.address;
        m_blocks[index].tag = req.tag;
        m_blocks[index].valid = true;
        m_blocks[index].dirty = m_blocks[index].dirty ? m_blocks[index].dirty : req.type == RequestType::Write || req.type == RequestType::DirtyWrite;
        return resp;
    }

    void Set::PrintContents(FILE* file){
        SortBlocks();
        for(auto &b : m_blocks){
            b.PrintContents(file);
        }

    }


    void Set::SortBlocks(){
        std::sort(m_blocks.begin(), m_blocks.end(),[](const Block &a,const Block &b){return a.counter < b.counter;});
    }

    void Set::IncLessThan(uint32_t threshold) {
        for(auto &b : m_blocks){
            if(b.counter < threshold) b.counter++;
        }
    }
} // CacheSim