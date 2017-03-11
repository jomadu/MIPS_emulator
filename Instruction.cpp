//
//  Instruction.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include <iostream>
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




