//
//  Cache.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/17/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "Cache.hpp"
#include "Memory.hpp"


Cache::Cache(){}

Cache::Cache(int nSets, int sSize, int penalty, Memory &mem, string nm){
    numSets = nSets;
    sets = new Set[numSets];
    setSize = sSize;
    missPenalty = penalty-1;
    name = nm;
    inPenalty = 0;
    haultPipeline = false;
    
    numByteOffsetBits = ceil(log2(WORD_SIZE/8));
    numBlockOffsetBits = ceil(log2(setSize));
    numIdxBits = ceil(log2(nSets));
    numTagBits = WORD_SIZE-numIdxBits-numBlockOffsetBits-numByteOffsetBits;
    
    tagMask = (0xFFFFFFFF << (WORD_SIZE-numTagBits));
    idxMask = (0xFFFFFFFF >> (WORD_SIZE-numIdxBits)) << (numBlockOffsetBits+numByteOffsetBits);
    if (numBlockOffsetBits == 0){
        blockOffsetMask = 0x0;
    }
    else{
        blockOffsetMask = (0xFFFFFFFF >> (WORD_SIZE-numBlockOffsetBits)) << (numByteOffsetBits);
    }
    byteOffsetMask = (0xFFFFFFFF >> (WORD_SIZE-numByteOffsetBits));
    
}
bool Cache::checkData(unsigned int addr){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    if (sets[idx].valid){
        return true;
    }
    return false;
}
bool Cache::loadW(unsigned int &ret, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we inPenalty?
    // No
    // Is the data valid with correct tag?
    // Yes
    // return the data and true
    // No
    // load the data into the iCache
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
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            ret = sets[idx].data;
            haultPipeline = false;
            return true;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n", name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            inPenalty = missPenalty;
            
            ret = sets[idx].data;
            haultPipeline = true;
            return false;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), inPenalty);
        }
        inPenalty--;
        if (!inPenalty){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        
        ret = sets[idx].data;
        haultPipeline = true;
        return false;
    }
}
bool Cache::storeW(unsigned int dataW, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we inPenalty
    // No
    //   Is the data at idx valid and have the correct tag?
    //   Yes
    //     Update the data with dataW
    //   No
    //     We need to store valid data in fill range
    //     Bring in new blocks
    //     Then update the data with dataW
    //     Start Penalty
    // Yes
    //   Decrement the inPenalty Counter
    //   After decrementing, is the inPenalty Counter now 0?
    //   Yes
    //     validate the loaded data, so that next time
    //     we update the data with dataW
    //   return false
    
    if (!inPenalty){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            sets[idx].data = dataW;
            haultPipeline = false;
            return true;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n",name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            inPenalty = missPenalty;
            haultPipeline = true;
            return false;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), inPenalty);
        }
        inPenalty--;
        if (!inPenalty){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        haultPipeline = true;
        return false;
    }
}

void Cache::blockFill(unsigned int addr, unsigned int nLines, Memory mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    unsigned int memAddr;
    
    for (int i = 0; i < nLines; i++){
        decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
        if (sets[idx].valid){
            if (debug){
                printf("%s: Storing valid data at idx: 0x%X to memory.\n",name.c_str(), idx);
            }
            memAddr = encodeCacheAddr(tag, idx, blockOffset, byteOffset);
            mem.storeW(sets[idx].data, memAddr);
        }
        sets[idx].data = mem.loadW(addr);
        sets[idx].tag = tag;
        sets[idx].valid = false;
        addr = addr + 4;
    }
}

void Cache::validateBlocks(unsigned int addr, unsigned int nLines){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    for (int i = 0; i < nLines; i++){
        decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
        sets[idx].valid = true;
        addr = addr + 4;
    }
}

void Cache::flush(){
    for (int i = 0; i < numSets; i++){
        sets[i].valid = false;
    }
}

void Cache::print(){
    printf("|----------------------------------------|\n"
           "| %-39s                                  |\n"
           "|----------|---|------------|------------|\n"
           "| idx      | v | tag        | data       |\n"
           "|----------|---|------------|------------|\n",name.c_str());
    
    for (unsigned int i = 0; i < numSets; i++){
        printf("| 0x%-6X | %d | 0x%-8X | 0x%-8X |\n",
               i,
               sets[i].valid,
               sets[i].tag,
               sets[i].data);
    }
    printf("|----------|---|------------|------------|\n\n");
}

void Cache::decodeCacheAddr(unsigned int &tag, unsigned int &idx, unsigned int &blkOffset, unsigned int &bOffset, unsigned int addr){
    tag = (tagMask & addr) >> (numIdxBits + numBlockOffsetBits + numByteOffsetBits);
    idx = (idxMask & addr) >> (numBlockOffsetBits + numByteOffsetBits);
    blkOffset = (blockOffsetMask & addr) >> (numByteOffsetBits);
    bOffset = (byteOffsetMask & addr);
}

unsigned int Cache::encodeCacheAddr(unsigned int tag, unsigned int idx, unsigned int blkOffset, unsigned int bOffset){
    unsigned int addr;
    
    addr = (tag << (numIdxBits + numBlockOffsetBits + numByteOffsetBits)) |
            (idx << (numBlockOffsetBits + numByteOffsetBits)) |
            (blkOffset << (numByteOffsetBits)) |
            (bOffset);
    return addr;
}

