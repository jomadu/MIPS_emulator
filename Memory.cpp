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
    memStart = INIT_MEMSTART;
    instrEnd = INIT_MEMSTART;
}

Memory::Memory(char * iFile, unsigned int &pc){
    mem = new unsigned int [MEMSIZE/4];
    memStart = INIT_MEMSTART;
    instrEnd = INIT_MEMSTART;
    importFile(iFile, pc);
}

unsigned int Memory::fetchInstr(unsigned int addr){
    // Fetch instruction mc from low memory
    int memIdx = addr2idx(addr);
    return mem[memIdx];
}

void Memory::storeInstr(unsigned int mc, unsigned int addr){
    // Store instruction mc in low memory
    int memIdx = addr2idx(addr);
    mem[memIdx] = mc;
    return;
}

unsigned int Memory::loadData(unsigned int addr){
    // Load data from high memory
    // FUTURE TODO: This may screw things up royally (off by one error?)
    int memIdx = MEMSIZE - addr2idx(addr);
    return mem[memIdx];
}

void Memory::storeData(unsigned int data, unsigned int addr){
    // Store data to high memory
    // FUTURE TODO: SEE ABOVE OR BEAR THE CONSEQUENCES
    int memIdx = MEMSIZE - addr2idx(addr);
    mem[memIdx] = data;
    return;
}

int Memory::addr2idx(unsigned int addr){
    return (addr - memStart)/4;
}

void Memory::importFile(char * iFile, unsigned int &pc){
    char buffer[80];
    char cAddr[80];
    char cData[80];
    string sAddr;
    string sData;
    unsigned long lAddr = 0x0;
    unsigned long lData = 0x0;
    unsigned int iAddr = 0x0;
    unsigned int iData = 0x0;
    unsigned int intMask = 0xFFFFFFFF;

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
            sscanf(buffer, "%s  %s", cAddr, cData);
            
            ss << cAddr;
            ss >> sAddr;
            lAddr = stoul(sAddr.substr(2,8),nullptr,16);
            iAddr = (lAddr & intMask);
            
            ss.clear();
            
            ss << cData;
            ss >> sData;
            lData = stoul(sData.substr(2,8),nullptr,16);
            iData = (lData & intMask);
            
            ss.clear();
            
            if(firstLine){
                memStart = iAddr;
                pc = iAddr;
                firstLine = false;
            }
            
            storeInstr(iData, iAddr);
            
        }
        instrEnd = iAddr;
    }
}

void Memory::printInstr(unsigned int startAddr, unsigned int size){
    unsigned int endAddr = startAddr + (size * 4);
    unsigned int memIdx;
    
    printf("\nMemory (Instructions)\n"
           "-------------\n"
           "...\n");
    for (unsigned int i = endAddr; i >= startAddr; i = i - 4){
        memIdx = addr2idx(i);
        printf("0x%X:\t0x%X\n", i, mem[memIdx]);
    }
}
void Memory::printData(unsigned int startAddr, unsigned int size){
    int endAddr = startAddr - (size * 4);
    int memIdx;
    
    printf("\nMemory (Instructions)\n"
           "-------------\n");
    for (int i = startAddr; i >= endAddr; i = i - 4){
        memIdx = MEMSIZE - addr2idx(i);
        printf("0x%X:\t0x%X\n", i, mem[memIdx]);
    }
    printf("...\n");
}

