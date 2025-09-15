//
// Created by Aweso on 9/8/2025.
//

#include "Set.h"

namespace CacheSim {

    Set::Set(uint32_t ways, uint32_t blocksize) :
    m_blocksize(blocksize) {
        m_blocks.resize(ways);
        for(int i = 0; i < ways; i++){
            m_blocks[i].counter = i;
        } // set counters to 0-MAX
    }

    SetResponse Set::SendRequest(SetRequest req) {
        SetResponse resp = {false,false,0};
        int index = 0;

        for(int i = 0; i <m_blocks.size() ;i++){ // check for tag matches
            if(m_blocks[i].valid && m_blocks[i].tag == req.tag){ // if the tag matches
                resp.hit = true; // hit
                index = i; // target block
                goto done;
            }
            index = m_blocks[i].counter > m_blocks[index].counter // else target block is LRU
                    ? m_blocks[i].counter
                    : m_blocks[index].counter;
        }


        done:
        if(!resp.hit) { //miss, eviction
            if(m_blocks[index].valid && m_blocks[index].dirty) { //we are evicting a dirty block
                resp.dirty = true;
                resp.dirty_tag = m_blocks[index].tag;
                m_blocks[index].dirty = false;
            }
        }
        IncLessThan(m_blocks[index].counter);
        m_blocks[index].counter = 0;
        m_blocks[index].tag = req.tag;
        m_blocks[index].valid = 1;
        m_blocks[index].dirty = m_blocks[index].dirty ? m_blocks[index].dirty : req.type == RequestType::Write || req.type == RequestType::DirtyWrite;

        return resp;
    }

    void Set::PrintContents(FILE* file){
        for(auto &b : m_blocks){
            b.PrintContents(file);
        }
    }


    void Set::IncLessThan(uint32_t threshold) {
        for(auto &b : m_blocks){
            if(b.counter < threshold) b.counter++;
        }
    }
} // CacheSim