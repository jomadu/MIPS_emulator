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
    penaltyCounter = 0;
    inPenalty = false;
    
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

unsigned int Cache::loadW(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
    // No
    //   Is the data valid with correct tag?
    //   Yes
    //     return the data and true
    //   No
    //     load the data into the iCache
    //     start the penaltyCounter Counter
    //     return the data at block idx and false
    // Yes
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     set the loaded data valid bits true
    //     so that next time, we return valid data
    //   return the data at block idx and false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            inPenalty = false;
            return sets[idx].data;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n", name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            
            inPenalty = true;
            return sets[idx].data;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        
        inPenalty = true;
        return sets[idx].data;
    }
}

unsigned int Cache::loadHWU(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    unsigned int ret;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
    // No
    //   Is the data valid with correct tag?
    //   Yes
    //     return the data and true
    //   No
    //     load the data into the iCache
    //     start the penaltyCounter Counter
    //     return the data at block idx and false
    // Yes
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     set the loaded data valid bits true
    //     so that next time, we return valid data
    //   return the data at block idx and false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            inPenalty = false;
            switch(byteOffset % 4){
                case 0:
                    return (HWL_MASK & sets[idx].data) >> 0;
                case 1:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWL_MASK & sets[idx].data) >> 0;
                case 2:
                    return (HWH_MASK & sets[idx].data) >> 16;
                case 3:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWH_MASK & sets[idx].data) >> 16;
                default:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWL_MASK & sets[idx].data) >> 0;
            }

        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n", name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            inPenalty = true;
            switch(byteOffset % 4){
                case 0:
                    return (HWL_MASK & sets[idx].data) >> 0;
                case 1:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWL_MASK & sets[idx].data) >> 0;
                case 2:
                    return (HWH_MASK & sets[idx].data) >> 16;
                case 3:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWH_MASK & sets[idx].data) >> 16;
                default:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    return (HWL_MASK & sets[idx].data) >> 0;
            }
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        inPenalty = true;
        switch(byteOffset % 4){
            case 0:
                return (HWL_MASK & sets[idx].data) >> 0;
            case 1:
                printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                return (HWL_MASK & sets[idx].data) >> 0;
            case 2:
                return (HWH_MASK & sets[idx].data) >> 16;
            case 3:
                printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                return (HWH_MASK & sets[idx].data) >> 16;
            default:
                printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                return (HWL_MASK & sets[idx].data) >> 0;
        }
    }
}

int Cache::loadHW(unsigned int addr, Memory &mem){
    unsigned int data;
    
    data = loadHWU(addr, mem);
    
    if (data >= 0x8000){
        return (0xFFFF0000 | data);
    }
    else{
        return data;
    }
    
}

unsigned int Cache::loadBU(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    unsigned int ret;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
    // No
    //   Is the data valid with correct tag?
    //   Yes
    //     return the data and true
    //   No
    //     load the data into the iCache
    //     start the penaltyCounter Counter
    //     return the data at block idx and false
    // Yes
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     set the loaded data valid bits true
    //     so that next time, we return valid data
    //   return the data at block idx and false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            inPenalty = false;
            switch(byteOffset % 4){
                case 0:
                    return (BYTE0_MASK & sets[idx].data) >> 0;
                case 1:
                    return (BYTE1_MASK & sets[idx].data) >> 8;
                case 2:
                    return (BYTE2_MASK & sets[idx].data) >> 16;
                case 3:
                    return (BYTE3_MASK & sets[idx].data) >> 24;
                default:
                    return (BYTE0_MASK & sets[idx].data) >> 0;
            }
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n", name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            inPenalty = true;

            switch(byteOffset % 4){
                case 0:
                    return (BYTE0_MASK & sets[idx].data) >> 0;
                case 1:
                    return (BYTE1_MASK & sets[idx].data) >> 8;
                case 2:
                    return (BYTE2_MASK & sets[idx].data) >> 16;
                case 3:
                    return (BYTE3_MASK & sets[idx].data) >> 24;
                default:
                    return (BYTE0_MASK & sets[idx].data) >> 0;
            }
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        inPenalty = true;
        switch(byteOffset % 4){
            case 0:
                return (BYTE0_MASK & sets[idx].data) >> 0;
            case 1:
                return (BYTE1_MASK & sets[idx].data) >> 8;
            case 2:
                return (BYTE2_MASK & sets[idx].data) >> 16;
            case 3:
                return (BYTE3_MASK & sets[idx].data) >> 24;
            default:
                return (BYTE0_MASK & sets[idx].data) >> 0;
        }
    }
}

int Cache::loadB(unsigned int addr, Memory &mem){
    unsigned int data;
    
    data = loadBU(addr, mem);
    
    if (data >= 0x8000){
        return (0xFFFF0000 | data);
    }
    else{
        return data;
    }
}

void Cache::storeW(unsigned int dataW, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
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
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     validate the loaded data, so that next time
    //     we update the data with dataW
    //   return false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            sets[idx].data = dataW;
            inPenalty = false;
            return;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n",name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            inPenalty = true;
            return;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        inPenalty = true;
        return;
    }
}

void Cache::storeHW(unsigned int dataHW, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
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
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     validate the loaded data, so that next time
    //     we update the data with dataW
    //   return false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            
            switch(byteOffset % 4){
                case 0:
                    sets[idx].data = (HWL_MASK & dataHW) >> 0;
                    break;
                case 1:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    sets[idx].data = (HWL_MASK & dataHW) >> 0;
                    break;
                case 2:
                    sets[idx].data = (HWH_MASK & dataHW) >> 16;
                    break;
                case 3:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    sets[idx].data = (HWH_MASK & dataHW) >> 16;
                    break;
                default:
                    printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
                    break;
            }
            inPenalty = false;
            return;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n",name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            inPenalty = true;
            return;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        inPenalty = true;
        return;
    }
}

void Cache::storeB(unsigned int dataB, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
    
    // Are we penaltyCounter?
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
    //   Decrement the penaltyCounter Counter
    //   After decrementing, is the penaltyCounter Counter now 0?
    //   Yes
    //     validate the loaded data, so that next time
    //     we update the data with dataW
    //   return false
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && (sets[idx].tag == tag)){
            if (debug){
                printf("%s: Hit!\n\n", name.c_str());
            }
            
            switch(byteOffset % 4){
                case 0:
                    sets[idx].data = (BYTE0_MASK & dataB) >> 0;
                    break;
                case 1:
                    sets[idx].data = (BYTE1_MASK & dataB) >> 8;
                    break;
                case 2:
                    sets[idx].data = (BYTE2_MASK & dataB) >> 16;
                    break;
                case 3:
                    sets[idx].data = (BYTE3_MASK & dataB) >> 24;
                    break;
                default:
                    break;
            }
            inPenalty = false;
            return;
        }
        else{
            if (debug){
                printf("%s: Miss! Block filling and starting penalty.\n\n",name.c_str());
            }
            blockFill(addr, NUM_BLOCK_FILL, mem);
            penaltyCounter = missPenalty;
            inPenalty = true;
            return;
        }
    }
    else{
        if (debug){
            printf("%s: Miss! At Penalty = %i\n\n", name.c_str(), penaltyCounter);
        }
        penaltyCounter--;
        if (penaltyCounter == 0){
            if (debug){
                printf("%s: Penalty over. Validating lines for next request.\n\n", name.c_str());
            }
            validateBlocks(addr, NUM_BLOCK_FILL);
        }
        inPenalty = true;
        return;
    }
}

void Cache::blockFill(unsigned int addr, unsigned int nSets, Memory mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int blockOffset;
    unsigned int byteOffset;
    unsigned int memAddr;
    
    for (int i = 0; i < nSets; i++){
        decodeCacheAddr(tag, idx, blockOffset, byteOffset, addr);
        if (sets[idx].valid){
            if (debug){
                printf("%s: Storing valid data at idx: 0x%X to memory.\n",name.c_str(), idx);
            }
            memAddr = encodeCacheAddr(sets[idx].tag, idx, blockOffset, byteOffset);
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
           "| %-39s|\n"
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

