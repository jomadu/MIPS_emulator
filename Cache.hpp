//
//  Cache.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/7/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Cache_hpp
#define Cache_hpp

#include <stdio.h>
#include <math.h>
#include "Memory.hpp"

#define DEFAULT_CACHE_SIZE 16
#define DEFAULT_MISS_PENALTY 4
#define NUM_BLOCK_FILL 4

#define BYTE0_MASK 0x000000FF
#define BYTE1_MASK 0x0000FF00
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0xFF000000

#define DEBUG true

class Cache{
public:
    struct Block{
        bool valid = false;
        unsigned int tag = 0x0;
        unsigned int offset = 0x0;
        unsigned int data = 0x0;
    };
    
    Block * blocks;
    int cacheSize;
    unsigned int missPenalty;
    int numIdxBits;
    int numTagBits;
    int numOffsetBits;
    unsigned int tagMask;
    unsigned int idxMask;
    unsigned int offsetMask;
    int inPenalty;
    
public:
    Cache();
    Cache(int size, int penalty, Memory &mem);
    bool checkData(unsigned int addr);
    bool loadData(unsigned int &ret, unsigned int addr, Memory &mem);
    void flush();
    void print();
private:
    void blockFill(unsigned int addr, unsigned int nLines, Memory mem);
    void validateBlocks(unsigned int addr, unsigned int nLines);
    unsigned int getTag(unsigned int addr);
    unsigned int getIdx(unsigned int addr);
    unsigned int getOffset(unsigned int addr);
    
};
#endif /* Cache_hpp */
