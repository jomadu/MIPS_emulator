//
//  IDEX_PR.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "IDEX_PR.hpp"

IDEX_PR::IDEX_PR(){
    regWrite = false;
    memToReg = false;
    branch = false;
    memWrite = false;
    memRead = false;
    regDst = false;
    ALUOp1 = false;
    ALUOp0 = false;
    ALUSrc = false;
    pcplus4 = 0x0;
    regFileReadData1 = 0x0;
    regFileReadData2 = 0x0;
    signExtend = 0x0;
}
