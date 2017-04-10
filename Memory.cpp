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

unsigned int Memory::fetch(unsigned int addr){
    // Fetch instruction mc from low memory
    int memIdx = addr2idx(addr);
    return mem[memIdx];
}

void Memory::store(unsigned int mc, unsigned int addr){
    // Store instruction mc in low memory
    int memIdx = addr2idx(addr);
    mem[memIdx] = mc;
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
            
            store(iData, addr);
            addr += 4;
        }
    }
}

void Memory::print(unsigned int startAddr, unsigned int size){
    unsigned int endAddr = startAddr + (size * 4);
    unsigned int memIdx;
    
    printf("\nMemory\n"
           "-------------\n"
           "...\n");
    for (unsigned int i = endAddr; i >= startAddr; i = i - 4){
        memIdx = addr2idx(i);
        printf("0x%X:\t0x%X\n", i, mem[memIdx]);
    }
}

