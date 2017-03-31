//
//  Instruction.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Instruction_hpp
#define Instruction_hpp

#include <stdio.h>
#include <iostream>

using namespace std;

class Instruction{
public:
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    int shamt;
    unsigned int funct;
    int immed;
    unsigned int addr;
    string type;
public:
    Instruction();
    void toNOP();
    void print();
};

#endif /* Instruction_hpp */
