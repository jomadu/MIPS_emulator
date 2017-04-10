//
//  Cache.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/7/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "Cache.hpp"
#include "Memory.hpp"

Cache::Cache(){
    cacheSize = DEFAULT_CACHE_SIZE;
    missPenalty = DEFAULT_MISS_PENALTY-1;
    inPenalty = 0;
    blocks = new Block[cacheSize];
}

Cache::Cache(int size, int penalty, Memory &mem){
    cacheSize = size;
    missPenalty = penalty-1;
    inPenalty = 0;
    blocks = new Block[cacheSize];
    
    //TODO: implement check to see if resulting
    
    numIdxBits = ceil(log2(size));
    numTagBits = 32 - numIdxBits;
    
    // Is numTagBits > ceil(log2(mem.memSize));
    if (numTagBits < ceil(log2(mem.size))){
        perror("Cache size is too small. Not enough bits to differentiate between all blocks.");
    }
    
    idxMask = 0xFFFFFFFF >> (numTagBits);
    tagMask = 0xFFFFFFFF << (numIdxBits);

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
            ret = blocks[idx].data;
            return true;
        }
        else{
            blockFill(addr, NUM_BLOCK_FILL, mem);
            inPenalty = missPenalty;
            ret = blocks[idx].data;
            return false;
        }
    }
    else{
        inPenalty--;
        if (!inPenalty){
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        ret = blocks[idx].data;
        return false;
    }
}

void Cache::blockFill(unsigned int addr, unsigned int nLines, Memory mem){
    unsigned int idx;
    unsigned int tag;
    
    for (int i = 0; i < nLines; i++){
        idx = getIdx(addr);
        tag = getTag(addr);
        blocks[idx].data = mem.fetch(addr);
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

unsigned int Cache::getTag(unsigned int addr){
    return (addr & tagMask) >> numIdxBits;
}

unsigned int Cache::getIdx(unsigned int addr){
    return (addr & idxMask) >> numIdxBits;
}
