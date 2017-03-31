//
//  EXMEM_PR.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef EXMEM_PR_hpp
#define EXMEM_PR_hpp

#include <stdio.h>
#include "Instruction.hpp"

class EXMEM_PR{
public:
    Instruction instr;
    bool regWrite;
    bool memToReg;
    bool branch;
    bool memWrite;
    bool memRead;
    bool ALUCompare;
    unsigned int ALUResult;
    unsigned int memWriteData;
    unsigned int regFileWriteReg;
public:
    EXMEM_PR();
};

#endif /* EXMEM_PR_hpp */
