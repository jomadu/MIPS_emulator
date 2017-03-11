//
//  EXMEM_PR.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "EXMEM_PR.hpp"

EXMEM_PR::EXMEM_PR(){
    regWrite = false;
    memToReg = false;
    branch = false;
    memWrite = false;
    memRead = false;
    branchTarget = 0x0;
    ALUCompare = false;
    ALUResult = 0x0;
    memWriteData = 0x0;
    regFileWriteReg = 0x0;
}
