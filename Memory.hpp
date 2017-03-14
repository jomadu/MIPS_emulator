//
//  Memory.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Memory_hpp
#define Memory_hpp

#include <stdio.h>
#include <iostream>
using namespace std;

#define INIT_MEMSTART 0
#define MEMSIZE 5000

class Memory{
public:
    unsigned int * mem;
    unsigned int memStart;
    unsigned int instrEnd;
public:
    Memory();
    Memory(char * iFile, unsigned int &pc);
    unsigned int fetchInstr(unsigned int addr);
    void storeInstr(unsigned int mc, unsigned int addr);
    unsigned int loadData(unsigned int addr);
    void storeData(unsigned int data, unsigned int addr);
    int addr2idx(unsigned int addr);
    void importFile(char * iFile, unsigned int &pc);
    void printInstr(unsigned int startAddr, unsigned int size);
    void printData(unsigned int startAddr, unsigned int size);
};

#endif /* Memory_hpp */
