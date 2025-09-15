//
// Created by Aweso on 9/8/2025.
//

#ifndef ECE463_TYPES_H
#define ECE463_TYPES_H

#include <cinttypes>
#include <array>
#include <vector>
#include <list>
#include <iostream>

#define ADDRESS_SIZE 32

namespace CacheSim {

    // Compile time configurable global address size
#if (ADDRESS_SIZE==8)
    typedef uint8_t Address;
    #define ADDRESS_MAX UINT8_MAX
#elif (ADDRESS_SIZE==16)
    typedef uint16_t Address;
    #define ADDRESS_MAX UINT16_MAX
    #define ADDRESS_FORM_SPEC "%04x"
#elif (ADDRESS_SIZE==32)
    typedef uint32_t Address;
    #define ADDRESS_MAX UINT32_MAX
    #define ADDRESS_FORM_SPEC "%08x"
#elif (ADDRESS_SIZE==64)
    typedef uint64_t Address;
    #define ADDRESS_MAX UINT64_MAX
    #define ADDRESS_FORM_SPEC "%016x"
#endif

#define RESP_TO_STR(_resp) (_resp ? "Hit" : "Miss")



    struct SetResponse{
        bool hit; // true == hit, false == miss
        bool dirty;
        Address dirty_tag;
        SetResponse() : hit(false), dirty(false), dirty_tag(0){};
        SetResponse(bool h, bool d, Address t) : hit(h), dirty(d), dirty_tag(t) {};
    };

    struct CacheResponse {
        bool hit; // true == hit, false == miss

        CacheResponse(SetResponse sr){hit = sr.hit;}
        CacheResponse(bool b){hit = b;}
    };

    enum class RequestType {
        Write,
        Read,
        DirtyWrite
    };

#define REQ_TO_STR(_request) (_request.type == RequestType::Write ? "Write" : "Read")

    struct CacheRequest {
        RequestType type;
        Address address;

        void print(FILE *file = stdout) const{
            fprintf(file,"%s\t%x",REQ_TO_STR((*this)),address);
        }

        CacheRequest(RequestType t, Address add) : type(t),address(add){};
        CacheRequest(){};

    };

    enum class CacheType{
        Normal,
        Memory
    };

    struct SetRequest{
        RequestType type;
        uint32_t tag, index; // block offset not needed

        SetRequest(RequestType rt, uint32_t t = 0, uint32_t i = 0) : type(rt), tag(t), index(i){}
        SetRequest(){}

        void print(FILE *file = stdout) const{
            fprintf(file,"%s\t%x\t%x\n",REQ_TO_STR((*this)),tag,index);
        }
    };

    struct BitWidths{
        uint8_t tag, index, blockOffset;
    };

    struct DebugInfo{
        CacheResponse cacresp;
        CacheRequest cacreq;
        SetResponse setresp;
        SetRequest setreq;
        std::string name;

        DebugInfo(CacheResponse cacheResponse, CacheRequest cacheRequest, SetResponse setResponse, SetRequest setRequest, std::string &newName) :
                cacresp(cacheResponse), cacreq(cacheRequest), setresp(setResponse), setreq(setRequest), name(newName){};

    };

    struct CacheStats{
        uint32_t read, read_miss, write, write_miss, writeback;
        CacheStats() : read(0), read_miss(0), write(0), write_miss(0), writeback(0) {};
        void print(FILE *file = stdout) {
            fprintf(file,"%u %u %u %u %f %u ",read, read_miss, write, write_miss, this->missRate(), writeback);
        }

        double missRate() const{
            return read + write == 0 ? 0 : ((double)read_miss + write_miss)/(read + write);
        }

        double readMissRate() const{
            return read == 0 ? 0 : ((double)read_miss)/(read);
        }

    };


}
#endif //ECE463_TYPES_H
