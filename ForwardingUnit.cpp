//
//  ForwardingUnit.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "ForwardingUnit.hpp"
#include "IDEX_PR.hpp"
#include "EXMEM_PR.hpp"
#include "MEMWB_PR.hpp"

ForwardingUnit::ForwardingUnit(){
    forwardA = 0x0;
    forwardB = 0x0;
}

void ForwardingUnit::update(IDEX_PR idex_pr, EXMEM_PR exmem_pr, MEMWB_PR memwb_pr){
    // Note: May be a case where mem hazard forwarding overwrites an exection hazard forwarding code
    
    // Execution Hazard Forwarding Detection
    if ((exmem_pr.regWrite) && (exmem_pr.instr.rd != 0) && (exmem_pr.instr.rd == idex_pr.instr.rs)){
        forwardA = 0x2;
    }
    else if ((exmem_pr.regWrite) && (exmem_pr.instr.rd != 0) && (exmem_pr.instr.rd == idex_pr.instr.rt)){
        forwardB = 0x2;
    }
    
    // Memory Hazard Forwarding Detection
    if ((memwb_pr.regWrite) && (memwb_pr.instr.rd != 0) && !((exmem_pr.regWrite) && (exmem_pr.instr.rd != 0) && (exmem_pr.instr.rd != idex_pr.instr.rs)) && (memwb_pr.instr.rd == idex_pr.instr.rs)){
        forwardA = 0x1;
    }
    else if ((memwb_pr.regWrite) && (memwb_pr.instr.rd != 0) && !((exmem_pr.regWrite) && (exmem_pr.instr.rd != 0) && (exmem_pr.instr.rd != idex_pr.instr.rs)) && (memwb_pr.instr.rd == idex_pr.instr.rs)){
        forwardA = 0x1;
    }
}
