//
//  Cache.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/7/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "Cache.hpp"
#include "Memory.hpp"


Cache::Cache(){}

Cache::Cache(int size, int penalty, Memory &mem){
    cacheSize = size;
    missPenalty = penalty-1;
    inPenalty = 0;
    blocks = new Block[cacheSize];
    
    //TODO: implement check to see if resulting
    
    numOffsetBits = 2;
    numIdxBits = ceil(log2(size));
    numTagBits = 32 - numIdxBits - numOffsetBits;
    
    // Is numTagBits > ceil(log2(mem.memSize));
    if (numTagBits < ceil(log2(mem.size/cacheSize))){
        perror("Cache size is too small. Not enough bits to differentiate between all blocks.");
    }
    
    idxMask = (0xFFFFFFFF >> (numTagBits)) << numOffsetBits;
    tagMask = 0xFFFFFFFF << (numIdxBits + numOffsetBits);
    offsetMask = 0xFFFFFFFF >> (32 - numOffsetBits);

}
bool Cache::checkData(unsigned int addr){
    if (blocks[getIdx(addr)].valid){
        return true;
    }
    return false;
}
bool Cache::loadData(unsigned int &ret, unsigned int addr, Memory &mem){
    unsigned int idx = getIdx(addr);
    unsigned int tag = getTag(addr);
    unsigned int offset = getOffset(addr);
    
    // Are we inPenalty?
    // No
        // Is the data valid with correct tag?
        // Yes
            // return the data and true
        // No
            // load the data into the cache
            // start the inPenalty Counter
            // return the data at block idx and false
    // Yes
        // Decrement the inPenalty Counter
        // After decrementing, is the inPenalty Counter now 0?
        // Yes
            // set the loaded data valid bits true
            // so that next time, we return valid data
        // return the data at block idx and false
    
    if (!inPenalty){
        if (blocks[idx].valid && (blocks[idx].tag == tag)){
            if (DEBUG){
                printf("Cache Hit!\n");
            }
            switch (offset) {
                case 0x0:
                    ret = blocks[idx].data;
                    break;
                case 0x1:
                    ret = (blocks[idx].data & BYTE0_MASK) >> 0;
                    break;
                case 0x2:
                    ret = (blocks[idx].data & BYTE1_MASK) >> 8;
                    break;
                case 0x3:
                    ret = (blocks[idx].data & BYTE2_MASK) >> 16;
                    break;
                case 0x4:
                    ret = (blocks[idx].data & BYTE3_MASK) >> 24;
                    break;
                default:
                    break;
            }
            return true;
        }
        else{
            if (true){
                printf("Cache Miss! Block filling and starting penalty.\n");
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            inPenalty = missPenalty;
            switch (offset) {
                case 0x0:
                    ret = blocks[idx].data;
                    break;
                case 0x1:
                    ret = (blocks[idx].data & BYTE0_MASK) >> 0;
                    break;
                case 0x2:
                    ret = (blocks[idx].data & BYTE1_MASK) >> 8;
                    break;
                case 0x3:
                    ret = (blocks[idx].data & BYTE2_MASK) >> 16;
                    break;
                case 0x4:
                    ret = (blocks[idx].data & BYTE3_MASK) >> 24;
                    break;
                default:
                    break;
            }
            return false;
        }
    }
    else{
        if (DEBUG){
            printf("Cache Miss! At Penalty = %i\n", inPenalty);
        }
        inPenalty--;
        if (!inPenalty){
            if (DEBUG){
                printf("Penalty over. Validating lines for next request.\n");
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        switch (offset) {
            case 0x0:
                ret = blocks[idx].data;
                break;
            case 0x1:
                ret = (blocks[idx].data & BYTE0_MASK) >> 0;
                break;
            case 0x2:
                ret = (blocks[idx].data & BYTE1_MASK) >> 8;
                break;
            case 0x3:
                ret = (blocks[idx].data & BYTE2_MASK) >> 16;
                break;
            case 0x4:
                ret = (blocks[idx].data & BYTE3_MASK) >> 24;
                break;
            default:
                break;
        }
        return false;
    }
}

void Cache::blockFill(unsigned int addr, unsigned int nLines, Memory mem){
    unsigned int idx;
    unsigned int tag;
    
    for (int i = 0; i < nLines; i++){
        idx = getIdx(addr);
        tag = getTag(addr);
        blocks[idx].data = mem.loadW(addr);
        blocks[idx].tag = tag;
        blocks[idx].valid = false;
        addr = addr + 4;
    }
}

void Cache::validateBlocks(unsigned int addr, unsigned int nLines){
    unsigned int idx;
    
    for (int i = 0; i < nLines; i++){
        idx = getIdx(addr);
        blocks[idx].valid = true;
        addr = addr + 4;
    }
}

void Cache::flush(){
    for (int i = 0; i < cacheSize; i++){
        blocks[i].valid = false;
    }
}

void Cache::print(){
    printf("\nCache\n"
           "-------------\n"
           "idx | v | tag | data |\n"
           "----------------------\n");
    
    for (unsigned int i = 0; i < cacheSize; i++){
        printf("0x%X | %d | 0x%X | 0x%X |\n",
               i,
               blocks[i].valid,
               blocks[i].tag,
               blocks[i].data);
    }
}


unsigned int Cache::getTag(unsigned int addr){
    return (addr & tagMask) >> (numIdxBits + numOffsetBits);
}

unsigned int Cache::getIdx(unsigned int addr){
    return (addr & idxMask) >> numOffsetBits;
}

unsigned int Cache::getOffset(unsigned int addr){
    return (addr & offsetMask);
}
