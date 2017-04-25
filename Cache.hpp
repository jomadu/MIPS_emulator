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

#define NUM_BLOCK_FILL 4
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
        unsigned int blockOffset = 0x0;
        unsigned int byteOffset = 0x0;
        unsigned int data = 0x0;
    };
    
    Set * sets;
    int numSets;
    int setSize;
    int missPenalty;
    int inPenalty;
    bool haultPipeline;
    
private:
    string name;
    
    int numIdxBits;
    int numTagBits;
    int numBlockOffsetBits;
    int numByteOffsetBits;
    
    unsigned int tagMask;
    unsigned int idxMask;
    unsigned int blockOffsetMask;
    unsigned int byteOffsetMask;
    
public:
    Cache();
    Cache(int nSets, int bSize, int penalty, Memory &mem, string nm);
    bool loadW(unsigned int &ret, unsigned int addr, Memory &mem);
    bool storeW(unsigned int dataW, unsigned int addr, Memory &mem);
    bool loadHWU(unsigned int &ret, unsigned int addr, Memory &mem);
    bool storeHWU(unsigned int dataHW, unsigned int addr, Memory &mem);
    bool loadBU(unsigned int &ret, unsigned int addr, Memory &mem);
    bool storeBU(unsigned int dataB, unsigned int addr, Memory &mem);
    void flush();
    void print();
private:
    void blockFill(unsigned int addr, unsigned int nSets, Memory mem);
    void validateBlocks(unsigned int addr, unsigned int nSets);
    unsigned int getTag(unsigned int addr);
    unsigned int getIdx(unsigned int addr);
    unsigned int getByteOffset(unsigned int addr);
    
    void decodeCacheAddr(unsigned int &tag, unsigned int &idx, unsigned int &blkOffset, unsigned int &bOffset, unsigned int addr);
    unsigned int encodeCacheAddr(unsigned int tag, unsigned int idx, unsigned int blkOffset, unsigned int bOffset);
    
};
#endif /* iCache_hpp */
