//
//  IDEX_PR.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef IDEX_PR_hpp
#define IDEX_PR_hpp

#include <stdio.h>
#include "Instruction.hpp"

class IDEX_PR{
public:
    Instruction instr;
    bool regWrite;
    bool memToReg;
    bool branch;
    bool memWrite;
    bool memRead;
    bool regDst;
    bool ALUOp1;
    bool ALUOp0;
    bool ALUSrc;
    unsigned int pcplus4;
    unsigned int regFileReadData1;
    unsigned int regFileReadData2;
    unsigned int signExtend;
public:
    IDEX_PR();
};

#endif /* IDEX_PR_hpp */
