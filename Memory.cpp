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
}

Memory::Memory(char * iFile, unsigned int &pc){
    mem = new unsigned int [MEMSIZE/4];
    memStart = INIT_MEMSTART;
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
    int iAddr;
    int iData;

    FILE *fp;
    int i = 0;
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
            iAddr = stoi(sAddr.substr(2,8),nullptr,16);
            
            ss.clear();
            
            ss << cData;
            ss >> sData;
            iData = stoi(sData.substr(2,8),nullptr,16);
            
            ss.clear();
            
            if(i == 0){
                memStart = iAddr;
                pc = iAddr;
            }
            
            storeInstr(iData, iAddr);
            
            printf("%d: %s",i++,buffer);
        }
    }
}


