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
#include "Constants.hpp"
#include "Testbench.hpp"

class EXMEM_PR{
public:
    Instruction instr;
    bool regWrite;
    bool memToReg;
    bool memWrite;
    bool memRead;
    unsigned int ALUResult;
    unsigned int memWriteData;
    unsigned int regFileWriteReg;
public:
    EXMEM_PR();
    void print();
};

#endif /* EXMEM_PR_hpp */
