//
//  MEMWB_PR.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef MEMWB_PR_hpp
#define MEMWB_PR_hpp

#include <stdio.h>
#include "Instruction.hpp"

class MEMWB_PR{
public:
    Instruction instr;
    bool regWrite;
    bool memToReg;
    bool dataUnsigned;
    unsigned int memReadDataU;
    int memReadData;
    unsigned int memBypassDataU;
    int memBypassData;
    unsigned int regFileWriteReg;
public:
    MEMWB_PR();
    void print();
};

#endif /* MEMWB_PR_hpp */
