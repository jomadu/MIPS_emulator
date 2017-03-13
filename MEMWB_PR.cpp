//
//  MEMWB_PR.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "MEMWB_PR.hpp"

MEMWB_PR::MEMWB_PR(){
    regWrite = false;
    memToReg = false;
    memReadData = 0x0;
    memBypassData = 0x0;
    regFileWriteReg = 0x0;
}

void MEMWB_PR::print(){
    printf("------------------------\n"
           "MEMWB Pipeline Register\n"
           "------------------------\n"
           "regWrite:\t\t\t%s\n"
           "memToReg:\t\t\t%s\n"
           "memReadData:\t\t0x%X\n"
           "memBypassdata:\t\t0x%X\n"
           "regFileWriteReg:\t0x%X\n\n",
           regWrite ? "true":"false",
           memToReg ? "true":"false",
           memReadData,
           memBypassData,
           regFileWriteReg);
}
