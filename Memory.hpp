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
#define BYTE0_MASK 0x000000FF
#define BYTE1_MASK 0x0000FF00
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0xFF000000
#define HWL_MASK 0x0000FFFF
#define HWM_MASK 0x00FFFF00
#define HWH_MASK 0xFFFF0000

class Memory{
private:
    unsigned int * mem;
public:
    int size;
public:
    Memory();
    Memory(char * iFile, unsigned int &pc);
    unsigned int loadW(unsigned int addr);
    void storeW(unsigned int dataW, unsigned int addr);
    unsigned int loadHWU(unsigned int addr);
    int loadHW(unsigned int addr);
    void storeHW(unsigned int dataHW, unsigned int addr, int offset);
    unsigned int loadBU(unsigned int addr);
    int loadB(unsigned int addr);
    void storeBU(unsigned int dataB, unsigned int addr);
    int addr2idx(unsigned int addr);
    void importFile(char * iFile, unsigned int &pc);
    void print(unsigned int startAddr, unsigned int size);
    unsigned int getByteOffset(unsigned int addr);

};

#endif /* Memory_hpp */
