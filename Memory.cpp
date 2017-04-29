//
//  Memory.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "Memory.hpp"
#include <iostream>
#include <sstream>

using namespace std;

Memory::Memory(){
    mem = new unsigned int [MEMSIZE/4];
    size = MEMSIZE;
}

Memory::Memory(char * iFile, unsigned int &pc){
    mem = new unsigned int [MEMSIZE/4];
    importFile(iFile, pc);
    size = MEMSIZE;
}

unsigned int Memory::loadW(unsigned int addr){
    // Fetch instruction mc from low memory
    int memIdx = addr2idx(addr);
    return mem[memIdx];
}

unsigned int Memory::loadHWU(unsigned int addr){
    // Fetch instruction mc from low memory
    unsigned int byteOffset = getByteOffset(addr);
    int memIdx = addr2idx(addr);
    switch (byteOffset % 4) {
        case 0:
            return (mem[memIdx] & HWL_MASK) >> 0;
            break;
        case 1:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            return (mem[memIdx] & HWL_MASK) >> 0;
            break;
        case 2:
            return (mem[memIdx] & HWH_MASK) >> 16;
            break;
        case 3:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            return (mem[memIdx] & HWH_MASK) >> 16;
            break;
        default:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            return (mem[memIdx] & HWL_MASK) >> 0;
            break;
    }
}

int Memory::loadHW(unsigned int addr){
    unsigned int data;
    
    data = loadHWU(addr);
    
    if (data >= 0x8000){
        return (0xFFFF0000 | data);
    }
    else{
        return data;
    }
}

unsigned int Memory::loadBU(unsigned int addr){
    // Fetch instruction mc from low memory
    unsigned int byteOffset = getByteOffset(addr);
    int memIdx = addr2idx(addr);
    switch (byteOffset % 4) {
        case 0:
            return (mem[memIdx] & BYTE0_MASK) >> 0;
            break;
        case 1:
            return (mem[memIdx] & BYTE1_MASK) >> 8;
            break;
        case 2:
            return (mem[memIdx] & BYTE2_MASK) >> 16;
            break;
        case 3:
            return (mem[memIdx] & BYTE3_MASK) >> 24;
            break;
        default:
            break;
    }
    return mem[memIdx];
}

int Memory::loadB(unsigned int addr){
    unsigned int data;
    
    data = loadBU(addr);
    
    if (data >= 0x8000){
        return (0xFFFF0000 | data);
    }
    else{
        return data;
    }
}

void Memory::storeW(unsigned int dataW, unsigned int addr){
    // Store instruction mc in low memory
    int memIdx = addr2idx(addr);
    mem[memIdx] = dataW;
    return;
}

void Memory::storeHW(unsigned int dataHW, unsigned int addr){
    // Store instruction mc in low memory
    unsigned int byteOffset = getByteOffset(addr);
    int memIdx = addr2idx(addr);
    
    switch (byteOffset % 4) {
        case 0:
            mem[memIdx] = (mem[memIdx] & HWH_MASK) | (dataHW & HWL_MASK);
            break;
        case 1:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            mem[memIdx] = (mem[memIdx] & HWH_MASK) | (dataHW & HWL_MASK);
            break;
        case 2:
            mem[memIdx] = ((dataHW & HWL_MASK) << 16) | (mem[memIdx] & HWL_MASK);
            break;
        case 3:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            mem[memIdx] = ((dataHW & HWL_MASK) << 16) | (mem[memIdx] & HWL_MASK);
            break;
        default:
            printf("Halfword addr was not halfword aligned: 0x%X\n", addr);
            mem[memIdx] = (mem[memIdx] & HWH_MASK) | (dataHW & HWL_MASK);
            break;
    }
    return;
}

void Memory::storeB(unsigned int dataB, unsigned int addr){
    // Store instruction mc in low memory
    unsigned int byteOffset = getByteOffset(addr);
    int memIdx = addr2idx(addr);
    
    switch (byteOffset % 4) {
        case 0:
            mem[memIdx] = (mem[memIdx] & 0xFFFFFF00) | (dataB & BYTE0_MASK);
            break;
        case 1:
            mem[memIdx] = (mem[memIdx] & 0xFFFF00FF) | ((dataB & BYTE0_MASK) << 8);
            break;
        case 2:
            mem[memIdx] = (mem[memIdx] & 0xFF00FFFF) | ((dataB & BYTE0_MASK) << 16);
            break;
        case 3:
            mem[memIdx] = (mem[memIdx] & 0x00FFFFFF) | ((dataB & BYTE0_MASK) << 24);
            break;
        default:
            mem[memIdx] = (mem[memIdx] & 0xFFFFFF00) | (dataB & BYTE0_MASK);
            break;
    }
    return;
}

int Memory::addr2idx(unsigned int addr){
    return addr/4;
}

void Memory::importFile(char * iFile, unsigned int &pc){
    char buffer[80];
    char cData[80];
    string sData;
    unsigned long lData = 0x0;
    unsigned int iData = 0x0;
    unsigned int intMask = 0xFFFFFFFF;
    unsigned int addr = 0x0;
    
    FILE *fp;
    bool firstLine = true;
    stringstream ss;


    if ((fp = fopen(iFile,"r")) == NULL)
    {
        printf("Could not open %s\n",iFile);
        exit(1);
    }

    while ( !feof(fp))
    {
        // read in the line and make sure it was successful
        if (fgets(buffer,500,fp) != NULL)
        {
            sscanf(buffer, "%s", cData);
            
            ss << cData;
            ss >> sData;
            lData = stoul(sData.substr(2,8),nullptr,16);
            iData = (lData & intMask);
            
            ss.clear();
            
            if(firstLine){
                pc = addr;
                firstLine = false;
            }
            
            storeW(iData, addr);
            addr += 4;
        }
    }
}

void Memory::print(unsigned int startAddr, unsigned int size){
    unsigned int memIdx;
    int addr;
    
    printf("\nMemory\n"
           "-------------\n"
           "...\n");
    for (int i = size; i >= 0; i--){
        addr = startAddr + (i * 4);
        memIdx = addr2idx(addr);
        printf("0x%X:\t0x%X\n", addr, mem[memIdx]);
    }
}

unsigned int Memory::getByteOffset(unsigned int addr){
    return (0x00000003 & addr);
}

