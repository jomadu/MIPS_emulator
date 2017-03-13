//
//  Instruction.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "Instruction.hpp"
using namespace std;

Instruction::Instruction(){
    opcode = 0x0;
    rs = 0x0;
    rt = 0x0;
    rd = 0x0;
    shamt = 0x0;
    funct = 0x0;
    immed = 0x0;
    addr = 0x0;
}
void Instruction::print(){
    printf("\nInstruction:\n"
           "------------\n"
           "opcode:\t0x%X\n"
           "rs:\t\t0x%X\n"
           "rt:\t\t0x%X\n"
           "rd:\t\t0x%X\n"
           "shamt:\t0x%X\n"
           "funct:\t0x%X\n"
           "immed:\t0x%X\n"
           "addr:\t0x%X\n"
           "type:\t%s\n",
           opcode,
           rs,
           rt,
           rd,
           shamt,
           funct,
           immed,
           addr,
           type.c_str());
}




