//
//  Cache.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/17/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Cache_hpp
#define Cache_hpp

#include <stdio.h>
#include <math.h>
#include "Memory.hpp"
#include "Testbench.hpp"

#define WORD_SIZE 32

#define BYTE0_MASK 0x000000FF
#define BYTE1_MASK 0x0000FF00
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0xFF000000

#define debug true

class Cache{
public:
    struct Set{
        bool valid = false;
        unsigned int tag = 0x0;
        unsigned int byteOffset = 0x0;
        unsigned int data = 0x0;
    };
    
    Set * sets;
    int numSets;
    int totalPenalty;
    int readPenalty;
    int writePenalty;
    int penaltyCounter;
    bool inPenalty;
    
    bool writtenThrough;
    
private:
    string name;
    
    int numIdxBits;
    int numTagBits;
    int numByteOffsetBits;
    
    unsigned int tagMask;
    unsigned int idxMask;
    unsigned int byteOffsetMask;
    
public:
    Cache();
    Cache(int nBytes, Memory &mem, string nm);
    unsigned int loadW(unsigned int addr, Memory &mem);
    unsigned int loadHWU(unsigned int addr, Memory &mem);
    int loadHW(unsigned int addr, Memory &mem);
    unsigned int loadBU(unsigned int addr, Memory &mem);
    int loadB(unsigned int addr, Memory &mem);
    void storeW(unsigned int data, unsigned int addr, Memory &mem);
    void storeHW(unsigned int dataHW, unsigned int addr, Memory &mem);
    void storeB(unsigned int dataB, unsigned int addr, Memory &mem);
    void flush();
    void print();
private:
    void blockFill(unsigned int addr, unsigned int nSets, Memory mem);
    void validateBlocks(unsigned int addr, unsigned int nSets);
    void decodeCacheAddr(unsigned int &tag, unsigned int &idx, unsigned int &bOffset, unsigned int addr);
    unsigned int encodeCacheAddr(unsigned int tag, unsigned int idx, unsigned int bOffset);
    bool evalLoadInPenalty(unsigned int tag, unsigned int idx, unsigned int byteOffset, unsigned int addr, Memory mem);
    
};
#endif /* iCache_hpp */
