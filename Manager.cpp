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

    void Manager::Statistics(){
        CacheStats l1s, l2s, mems;
        for(auto &c : m_layers){
            if(c->GetName() == "L1") l1s = c->GetStats();
            else if(c->GetName() == "L2") l2s = c->GetStats();
            else if(c->GetName() == "Memory") mems = c->GetStats();
        }

        printf("L1 Read, Read_miss, Write, Write_miss, MR, WB, 0, L2 Read, Read_miss, 0, 0, Write, Write_miss, MR, WB, 0, Total\n");
        printf("%u %u %u %u %f %u 0 %u %u 0 0 %u %u %f %u 0 %u",
               l1s.read, l1s.read_miss, l1s.write, l1s.write_miss, l1s.missRate(), l1s.writeback,
               l2s.read, l2s.read_miss, l2s.write, l2s.write_miss, l2s.readMissRate(), l2s.writeback,
               mems.write + mems.read);

        printf("\n");
        mems.print();

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