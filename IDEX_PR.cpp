//
//  IDEX_PR.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "IDEX_PR.hpp"
#include <stdio.h>
#include <iostream>
#include "Constants.hpp"
#include "Testbench.hpp"

IDEX_PR::IDEX_PR(){
    regDst = true;
    ALUOp0 = false;
    ALUOp1 = true;
    ALUSrc = false;
    branch = false;
    memRead = false;
    memWrite = false;
    regWrite = true;
    memToReg = false;
    pcnext = 0x0;
    regFileReadData1 = 0x0;
    regFileReadData2 = 0x0;
    signExtend = 0x0;
    zeroExtend = 0x0;
}

void IDEX_PR::print(){
    printf("------------------------\n"
           "IDEX Pipeline Register\n"
           "------------------------\n");
    instr.print();
    printf("\nOther\n"
           "------------\n");
    printf("regWrite:\t\t\t%s\n"
           "memToReg:\t\t\t%s\n"
           "branch:\t\t\t\t%s\n"
           "memWrite:\t\t\t%s\n"
           "memRead:\t\t\t%s\n"
           "regDst:\t\t\t\t%s\n"
           "ALUOP1:\t\t\t\t%s\n"
           "ALUOP0:\t\t\t\t%s\n"
           "ALUSrc:\t\t\t\t%s\n"
           "PCnext:\t\t\t\t0x%X\n"
           "regFileReadData1:\t0x%X\n"
           "regFileReadData2:\t0x%X\n"
           "signExtend:\t\t\t0x%X\n\n",
           regWrite ? "true":"false",
           memToReg ? "true":"false",
           branch ? "true":"false",
           memWrite ? "true":"false",
           memRead ? "true":"false",
           regDst ? "true":"false",
           ALUOp1 ? "true":"false",
           ALUOp0 ? "true":"false",
           ALUSrc ? "true":"false",
           pcnext,
           regFileReadData1,
           regFileReadData2,
           signExtend);
}
