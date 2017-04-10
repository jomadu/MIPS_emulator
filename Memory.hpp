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

#define MEMSIZE 5000

class Memory{
private:
    unsigned int * mem;
public:
    int size;
public:
    Memory();
    Memory(char * iFile, unsigned int &pc);
    unsigned int fetch(unsigned int addr);
    void store(unsigned int mc, unsigned int addr);
    int addr2idx(unsigned int addr);
    void importFile(char * iFile, unsigned int &pc);
    void print(unsigned int startAddr, unsigned int size);
};

#endif /* Memory_hpp */
