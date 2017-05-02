//
//  Cache.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/17/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "Cache.hpp"
#include "Memory.hpp"
#include "Testbench.hpp"
#include "Constants.hpp"

Cache::Cache(){}

Cache::Cache(int nBytes, Memory &mem, string nm){
    numSets = nBytes/4;
    sets = new Set[numSets];
    totalPenalty = 0;
    readPenalty = CACHE_R_STARTUP_P + (CACHE_SET_FILL-1)*CACHE_R_SUBSQT_P;
    writePenalty = CACHE_W_STARTUP_P + (CACHE_SET_FILL-1)*CACHE_W_SUBSQT_P;
    name = nm;
    penaltyCounter = 0;
    inPenalty = false;
    
    writtenThrough = false;
    
    numByteOffsetBits = ceil(log2(WORD_SIZE/8));
    numIdxBits = ceil(log2(numSets));
    numTagBits = WORD_SIZE-numIdxBits-numByteOffsetBits;
    
    tagMask = (0xFFFFFFFF << (WORD_SIZE-numTagBits));
    idxMask = (0xFFFFFFFF >> (WORD_SIZE-numIdxBits)) << (numByteOffsetBits);
    byteOffsetMask = (0xFFFFFFFF >> (WORD_SIZE-numByteOffsetBits));
    
    hitRate = 0;
    numAccesses = 0;
    numHits = 0;
    
}

unsigned int Cache::omnipotentRead(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
        
    return sets[idx].data;
    
}

unsigned int Cache::loadW(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
    
    inPenalty = evalLoadInPenalty(tag, idx, byteOffset, addr, mem);
    
    return sets[idx].data;
}

unsigned int Cache::loadHWU(unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
    
    inPenalty = evalLoadInPenalty(tag, idx, byteOffset, addr, mem);
    
    // Always return a value, even if we are in penalty.
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
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);

    inPenalty = evalLoadInPenalty(tag, idx, byteOffset, addr, mem);
    
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

void Cache::storeW(unsigned int data, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && sets[idx].tag == tag){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Hit!\n\n", name.c_str());
                }
                sets[idx].data = data;
                inPenalty = false;
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Hit!\n\n", name.c_str());
                    }
                    writtenThrough = false;
                    inPenalty = false;
                }
                else{
                    if (DEBUG){
                        printf("%s: Writing Through to Memory. Starting Penalty.\n", name.c_str());
                    }
                    sets[idx].data = data;
                    sets[idx].valid = false;
                    mem.storeW(data, addr);
                    totalPenalty = writePenalty;
                    penaltyCounter = totalPenalty - 1;
                    if (DEBUG){
                        printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
                    }
                    writtenThrough = true;
                    inPenalty = true;
                }
            }
        }
        else{
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (sets[idx].valid){
                    totalPenalty = writePenalty + readPenalty;
                }
                else{
                    totalPenalty = readPenalty;
                }
            }
            else{
                // Write Through
                totalPenalty = readPenalty;
            }
            if (DEBUG){
                printf("%s: Miss! Block Filling and Starting Penalty.\n", name.c_str());
            }
            penaltyCounter = totalPenalty - 1;
            if (DEBUG){
                printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
            }
            inPenalty = true;
            blockFill(addr, CACHE_SET_FILL, mem);
        }
    }
    else{
        penaltyCounter --;
        if (DEBUG){
            printf("%s: Miss! Penalty now at %i\n", name.c_str(), penaltyCounter);
        }
        if(penaltyCounter == 0){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                }
                validateBlocks(addr, CACHE_SET_FILL);
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating written through block.\n\n", name.c_str());
                    }
                    validateBlocks(addr, 1);
                }
                else{
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                    }
                    validateBlocks(addr, CACHE_SET_FILL);
                }
            }
        }
        inPenalty = true;
    }
    
    return;
}


void Cache::storeHW(unsigned int dataHW, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    unsigned int data;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
    
    // evaluate what data we'll be writing
    switch(byteOffset % 4){
        case 0:
            data = (sets[idx].data & HWH_MASK) | (dataHW & HWL_MASK);
            break;
        case 1:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            data = (sets[idx].data & HWH_MASK) | (dataHW & HWL_MASK);
            break;
        case 2:
            data = ((dataHW & HWL_MASK) << 16) | (sets[idx].data & HWL_MASK);
            break;
        case 3:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            data = ((dataHW & HWL_MASK) << 16) | (sets[idx].data & HWL_MASK);
            break;
        default:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            data = (sets[idx].data & HWH_MASK) | (dataHW & HWL_MASK);
            break;
    }
    
    if (penaltyCounter == 0){
        if (sets[idx].valid && sets[idx].tag == tag){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Hit!\n\n", name.c_str());
                }
                sets[idx].data = data;
                inPenalty = false;
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Hit!\n\n", name.c_str());
                    }
                    writtenThrough = false;
                    inPenalty = false;
                }
                else{
                    if (DEBUG){
                        printf("%s: Writing Through to Memory. Starting Penalty.\n", name.c_str());
                    }
                    sets[idx].data = data;
                    sets[idx].valid = false;
                    mem.storeHW(data, addr);
                    totalPenalty = writePenalty;
                    penaltyCounter = totalPenalty - 1;
                    if (DEBUG){
                        printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
                    }
                    writtenThrough = true;
                    inPenalty = true;
                }
            }
        }
        else{
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (sets[idx].valid){
                    totalPenalty = writePenalty + readPenalty;
                }
                else{
                    totalPenalty = readPenalty;
                }
            }
            else{
                // Write Through
                totalPenalty = readPenalty;
            }
            if (DEBUG){
                printf("%s: Miss! Block Filling and Starting Penalty.\n", name.c_str());
            }
            penaltyCounter = totalPenalty - 1;
            if (DEBUG){
                printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
            }
            inPenalty = true;
            blockFill(addr, CACHE_SET_FILL, mem);
        }
    }
    else{
        penaltyCounter --;
        if (DEBUG){
            printf("%s: Miss! Penalty now at %i\n", name.c_str(), penaltyCounter);
        }
        if(penaltyCounter == 0){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                }
                validateBlocks(addr, CACHE_SET_FILL);
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating written through block.\n\n", name.c_str());
                    }
                    validateBlocks(addr, 1);
                }
                else{
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                    }
                    validateBlocks(addr, CACHE_SET_FILL);
                }
            }
        }
        inPenalty = true;
    }
    
    return;
}

void Cache::storeB(unsigned int dataB, unsigned int addr, Memory &mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    unsigned int data;
    
    decodeCacheAddr(tag, idx, byteOffset, addr);
    
    // evaluate what data we'll be writing
    switch(byteOffset % 4){
        case 0:
            data = (sets[idx].data & 0xFFFFFF00) | (dataB & BYTE0_MASK);
            break;
        case 1:
            data = (sets[idx].data & 0xFFFF00FF) | ((dataB & BYTE0_MASK) << 8);
            break;
        case 2:
            data = (sets[idx].data & 0xFF00FFFF) | ((dataB & BYTE0_MASK) << 16);
            break;
        case 3:
            data = (sets[idx].data & 0x00FFFFFF) | ((dataB & BYTE0_MASK) << 24);
            break;
        default:
            data = (sets[idx].data & 0xFFFFFF00) | (dataB & BYTE0_MASK);
            break;
    }
    if (penaltyCounter == 0){
        if (sets[idx].valid && sets[idx].tag == tag){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Hit!\n\n", name.c_str());
                }
                sets[idx].data = data;
                inPenalty = false;
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Hit!\n\n", name.c_str());
                    }
                    writtenThrough = false;
                    inPenalty = false;
                }
                else{
                    if (DEBUG){
                        printf("%s: Writing Through to Memory. Starting Penalty.\n", name.c_str());
                    }
                    sets[idx].data = data;
                    sets[idx].valid = false;
                    mem.storeB(data, addr);
                    totalPenalty = writePenalty;
                    penaltyCounter = totalPenalty - 1;
                    if (DEBUG){
                        printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
                    }
                    writtenThrough = true;
                    inPenalty = true;
                }
            }
        }
        else{
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (sets[idx].valid){
                    totalPenalty = writePenalty + readPenalty;
                }
                else{
                    totalPenalty = readPenalty;
                }
            }
            else{
                // Write Through
                totalPenalty = readPenalty;
            }
            if (DEBUG){
                printf("%s: Miss! Block Filling and Starting Penalty.\n", name.c_str());
            }
            penaltyCounter = totalPenalty - 1;
            if (DEBUG){
                printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
            }
            inPenalty = true;
            blockFill(addr, CACHE_SET_FILL, mem);
            numAccesses ++;
        }
    }
    else{
        penaltyCounter --;
        if (DEBUG){
            printf("%s: Miss! Penalty now at %i\n", name.c_str(), penaltyCounter);
        }
        if(penaltyCounter == 0){
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (DEBUG){
                    printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                }
                validateBlocks(addr, CACHE_SET_FILL);
            }
            else{
                // Write Through
                if (writtenThrough){
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating written through block.\n\n", name.c_str());
                    }
                    validateBlocks(addr, 1);
                }
                else{
                    if (DEBUG){
                        printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
                    }
                    validateBlocks(addr, CACHE_SET_FILL);
                }
            }
        }
        inPenalty = true;
    }
    
    return;
}


void Cache::blockFill(unsigned int addr, unsigned int nSets, Memory mem){
    unsigned int idx;
    unsigned int tag;
    unsigned int byteOffset;
    unsigned int memAddr;
    
    for (int i = 0; i < nSets; i++){
        decodeCacheAddr(tag, idx, byteOffset, addr);
        if (CACHE_WRITE_POLICY == 0){
            if (sets[idx].valid){
                memAddr = encodeCacheAddr(sets[idx].tag, idx, byteOffset);
                if (DEBUG){
                    printf("%s: Storing valid data: 0x%X at idx: 0x%X to memory in addr: 0x%X.\n",name.c_str(),sets[idx].data,idx,memAddr);
                }
                mem.storeW(sets[idx].data, memAddr);
                if (DEBUG){
                    printf("%s: Memory at addr: 0x%X is now 0x%X\n",name.c_str(),memAddr,sets[idx].data);
                }
            }
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
    unsigned int byteOffset;
    
    for (int i = 0; i < nLines; i++){
        decodeCacheAddr(tag, idx, byteOffset, addr);
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
    unsigned int addr;
    printf("|------------------------------------------------------------------|\n"
           "| %-65s|\n"
           "|----------|---|------------|------------|------------|------------|\n"
           "| idx      | v | tag        | data       | addr       | memIdx     |\n"
           "|----------|---|------------|------------|------------|------------|\n",name.c_str());
    
    for (unsigned int i = 0; i < numSets; i++){
        addr = encodeCacheAddr(sets[i].tag, i, 0);
        printf("| 0x%-6X | %d | 0x%-8X | 0x%-8X | 0x%-8X | %-10i |\n",
               i,
               sets[i].valid,
               sets[i].tag,
               sets[i].data,
               addr,
               addr/4);
    }
    printf("|----------|---|------------|------------|------------|------------|\n\n");
}

void Cache::decodeCacheAddr(unsigned int &tag, unsigned int &idx, unsigned int &bOffset, unsigned int addr){
    tag = (tagMask & addr) >> (numIdxBits + numByteOffsetBits);
    idx = (idxMask & addr) >> (numByteOffsetBits);
    bOffset = (byteOffsetMask & addr);
}

unsigned int Cache::encodeCacheAddr(unsigned int tag, unsigned int idx, unsigned int bOffset){
    unsigned int addr;
    
    addr = (tag << (numIdxBits + numByteOffsetBits)) |
            (idx << (numByteOffsetBits)) |
            (bOffset);
    return addr;
}

bool Cache::evalLoadInPenalty(unsigned int tag, unsigned int idx, unsigned int byteOffset, unsigned int addr, Memory mem){
    if (penaltyCounter == 0){
        if (sets[idx].valid && sets[idx].tag == tag){
            if (DEBUG){
                printf("%s: Hit!\n\n", name.c_str());
            }
            return false;
        }
        else{
            if (CACHE_WRITE_POLICY == 0){
                // Write Back
                if (sets[idx].valid){
                    totalPenalty = writePenalty + readPenalty;
                }
                else{
                    totalPenalty = readPenalty;
                }
            }
            else{
                // Write Through
                totalPenalty = readPenalty;
            }
            if (DEBUG){
                printf("%s: Miss! Block Filling and Starting Penalty.\n", name.c_str());
            }
            penaltyCounter = totalPenalty - 1;
            if (DEBUG){
                printf("%s: Penalty at %i!\n\n", name.c_str(), penaltyCounter);
            }
            blockFill(addr, CACHE_SET_FILL, mem);
            return true;
        }
    }
    else{
        penaltyCounter--;
        if (DEBUG){
            printf("%s: Miss! Penalty now at %i\n", name.c_str(), penaltyCounter);
        }
        if (penaltyCounter == 0){
            if (DEBUG){
                printf("%s: Penalty Over! Validating previously filled blocks.\n\n", name.c_str());
            }
            validateBlocks(addr, CACHE_SET_FILL);
        }
        return true;
    }
}

void Cache::updateHitRate(int a, int h){
    hitRate = (float)h / (float)a;
    return;
}

