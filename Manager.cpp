//
// Created by Aweso on 9/10/2025.
//

#include "Manager.h"

namespace CacheSim {

    Manager::Manager(size_t layerCount){
        m_layers.resize(4);
        m_layers.clear();
    }

    Manager::~Manager(){
        for(auto l : m_layers) delete l;
    }

    void Manager::AppendLayer(const std::string &name, uint32_t SIZE, uint32_t ASSOC, uint32_t BLOCKSIZE, CacheType type){
        auto prev = m_layers.back();
        m_layers.emplace_back(new Cache(name,SIZE,ASSOC,BLOCKSIZE,type));
        if(prev) prev->SetNextLayer(m_layers.back());
    }

    void Manager::AppendLayer(const std::string& name, CacheType type){
        auto prev = m_layers.back();
        m_layers.emplace_back(new Cache(name,type));
        if(prev)  prev->SetNextLayer(m_layers.back());
    }

    void Manager::StartDebugLayers(){
        for(auto &l: m_layers){
            l->SetDebugVector(&m_results);
        }
    }

    void Manager::StopDebugLayers(){
        for(auto &l: m_layers){
            l->ResetDebugVector();
        }
    }

    void Manager::PrintResults(FILE *file){

        fprintf(file,"Name,Type,Address,Tag,Index,Hit,Dirty_evicted,Dirty_address\n");
        for(int i = 0; i < m_results.size();i++){
            fprintf(file,"%s,%s," ADDRESS_FORM_SPEC ",%x,%x,%s,%s,%x\n",
                    m_results[i].name.c_str(),
                    m_results[i].cacreq.type == RequestType::Write ? "Write" : m_results[i].cacreq.type == RequestType::DirtyWrite ? "DirtyWrite" : "Read",
                    m_results[i].cacreq.address,
                    m_results[i].setreq.tag,
                    m_results[i].setreq.index,
                    m_results[i].setresp.hit ? "Hit" : "Miss",
                    m_results[i].setresp.dirty ? "Dirty Evict" : "None",
                    m_results[i].setresp.dirty_tag
            );
        }
    }

    void Manager::PrintStatistics(FILE *file){
        CacheStats l1s, l2s, mems;
        for(auto &c : m_layers){
            if(c->GetName() == "L1") l1s = c->GetStats();
            else if(c->GetName() == "L2") l2s = c->GetStats();
            else if(c->GetName() == "Memory") mems = c->GetStats();
        }

        fprintf(file,"===== Measurements =====\n");
        fprintf(file,"a. L1 reads:                   %u\n",l1s.read);
        fprintf(file,"b. L1 read misses:             %u\n",l1s.read_miss);
        fprintf(file,"c. L1 writes:                  %u\n",l1s.write);
        fprintf(file,"d. L1 write misses:            %u\n",l1s.write_miss);
        fprintf(file,"e. L1 miss rate:               %.4f\n",l1s.missRate());
        fprintf(file,"f. L1 writebacks:              %u\n",l1s.writeback);
        fprintf(file,"g. L1 prefetches:              %u\n",0);
        fprintf(file,"h. L2 reads (demand):          %u\n",l2s.read);
        fprintf(file,"i. L2 read misses (demand):    %u\n",l2s.read_miss);
        fprintf(file,"j. L2 reads (prefetch):        %u\n",0);
        fprintf(file,"k. L2 read misses (prefetch):  %u\n",0);
        fprintf(file,"l. L2 writes:                  %u\n",l2s.write);
        fprintf(file,"m. L2 write misses:            %u\n",l2s.write_miss);
        fprintf(file,"n. L2 miss rate:               %.4f\n",l2s.missRate());
        fprintf(file,"o. L2 writebacks:              %u\n",l2s.writeback);
        fprintf(file,"p. L2 prefetches:              %u\n",0);
        fprintf(file,"q. memory traffic:             %u\n",mems.write+mems.read);

    }

    void Manager::PrintContents(FILE *file) {
        for(auto &c : m_layers){
            if(c->GetType()!=CacheType::Memory){
                c->PrintContents(file);
            }
        }
    }

    void Manager::ReadTrace(FILE *file, uint32_t lines){
        uint32_t lineCount = 0;
        while(!feof(file) && lineCount < lines){
            if(lineCount == 20){
                int x = 0;
            }
            char mode;
            uint32_t hex;
            fscanf(file,"%c %x\n",&mode,&hex);
            CacheRequest req;
            if(mode == 'r') req.type = RequestType::Read;
            else if(mode == 'w') req.type = RequestType::Write;
            else {
                printf("Error reading trace file\n");
                return;
            }
            req.address = hex;
            SendRequest(req);
            lineCount++;
        }

    }

    CacheResponse Manager::SendRequest(CacheRequest req){
        return m_layers.front()->SendRequest(req);
    }
} // CacheSim