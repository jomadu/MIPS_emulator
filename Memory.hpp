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
#include "Constants.hpp"
#include "Testbench.hpp"

using namespace std;

class Memory{
private:
    unsigned int * mem;
public:
    int size;
public:
    Memory();
    Memory(char * iFile);
    unsigned int loadW(unsigned int addr);
    unsigned int loadHWU(unsigned int addr);
    int loadHW(unsigned int addr);
    unsigned int loadBU(unsigned int addr);
    int loadB(unsigned int addr);
    void storeW(unsigned int dataW, unsigned int addr);
    void storeHW(unsigned int dataHW, unsigned int addr);
    void storeB(unsigned int dataB, unsigned int addr);
    int addr2idx(unsigned int addr);
    int importFile(char * iFile);
    void print(unsigned int startAddr, unsigned int size);
    unsigned int getByteOffset(unsigned int addr);

};

#endif /* Memory_hpp */
