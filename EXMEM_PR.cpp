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
    memWrite = false;
    memRead = false;
    ALUResult = 0x0;
    memWriteData = 0x0;
    regFileWriteReg = 0x0;
}

void EXMEM_PR::print(){
    printf("------------------------\n"
           "EXMEM Pipeline Register\n"
           "------------------------\n"
           "regWrite:\t\t\t%s\n"
           "memToReg:\t\t\t%s\n"
           "memWrite:\t\t\t%s\n"
           "memRead:\t\t\t%s\n"
           "ALUResult:\t\t\t0x%X\n"
           "memWriteData:\t\t0x%X\n"
           "regFileWriteReg:\t0x%X\n\n",
           regWrite ? "true":"false",
           memToReg ? "true":"false",
           memWrite ? "true":"false",
           memRead ? "true":"false",
           ALUResult,
           memWriteData,
           regFileWriteReg);
}
