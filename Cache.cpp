//
// Created by Aweso on 9/8/2025.
//

#include "Cache.h"
#include <math.h>

namespace CacheSim {
    Cache::Cache(const std::string &name, uint32_t SIZE, uint32_t ASSOC, uint32_t BLOCKSIZE, CacheType type) : m_name(name),
            m_size(SIZE), m_assoc(ASSOC), m_blockSize(BLOCKSIZE), m_type(type), m_results(nullptr) {
        m_bitWidths.blockOffset = log2(BLOCKSIZE);
        m_bitWidths.index = log2((SIZE/(ASSOC*BLOCKSIZE)));
        m_bitWidths.tag = ADDRESS_SIZE - m_bitWidths.index - m_bitWidths.blockOffset;
        m_sets.resize((SIZE/(ASSOC*BLOCKSIZE)),{ASSOC,BLOCKSIZE}); //Set(ways) -> Set(ASSOC)
    }

    Cache::Cache(const std::string& name, CacheType type) : m_name(name), m_type(type), m_results(nullptr){

    }


    CacheResponse Cache::SendRequest(const CacheSim::CacheRequest &req) {

        if(m_type==CacheType::Memory){
            switch(req.type){
                case RequestType::Read:
                    m_stats.read++;
                    break;
                case RequestType::Write:
                case RequestType::DirtyWrite:
                    m_stats.write++;
                    break;
            }
            if(m_results)
                m_results->emplace_back(
                        (true),
                        req,
                        SetResponse(true,false,0),
                        (req.type),
                        m_name);
            return {true};
        }

        if(m_name=="L2" && req.type != RequestType::Read){
            int x = 0;
        }


        auto sr = CacheToSetRequest(req);
        auto res = m_sets[sr.index].SendRequest(sr);

        // Log result
        if(m_results){
            m_results->emplace_back(res,req,res,sr,m_name);
        }

        switch(req.type){
            case RequestType::Read:
                m_stats.read++;
                if(!res.hit) m_stats.read_miss++;
                break;
            case RequestType::Write:
            case RequestType::DirtyWrite:
                m_stats.write++;
                if(!res.hit) m_stats.write_miss++;
                break;
        }



        if(res.hit || req.type != RequestType::Read) { // if we hit, return
            return res;
        } else{ //if we missed
            if(res.dirty) { // Dirty write, then normal read or nothing if write
                if(m_name=="L2" && req.type != RequestType::Read){
                    int x = 0;
                }
                m_stats.writeback++;
                auto dirty_req = req;
                dirty_req.type = RequestType::DirtyWrite;
                m_nextLayer->SendRequest(dirty_req);
            }
            return m_nextLayer->SendRequest(req);//try next cache
        }

    }

    CacheStats Cache::GetStats(){
        return m_stats;
    }

    std::string& Cache::GetName(){
        return m_name;
    }

    SetRequest Cache::CacheToSetRequest(const CacheSim::CacheRequest &req) const {
        SetRequest retval{};
        retval.type = req.type;
        retval.tag = req.address >> (m_bitWidths.index + m_bitWidths.blockOffset); // overwrite tag and offset
        retval.index = (req.address & (ADDRESS_MAX >> m_bitWidths.tag)) >> m_bitWidths.blockOffset; // zero tag, then shift tag right
        return retval;
    }
    void Cache::SetNextLayer(Cache* next){
        m_nextLayer = next;
    }

    void Cache::SetDebugVector(std::vector<DebugInfo> *results){
        m_results = results;
    }

    void Cache::ResetDebugVector(){
        m_results = nullptr;
    }

    CacheType Cache::GetType() {
        return m_type;
    }

    void Cache::PrintContents(FILE *file) {
        fprintf(file,"===== %s contents =====\n",m_name.c_str());
        size_t index = 0;
        for(auto &s : m_sets){
            fprintf(file,"set\t%llu:\t",index++);
            s.PrintContents(file);
            fprintf(file,"\n");
        }
    }
}