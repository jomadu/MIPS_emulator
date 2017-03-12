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
