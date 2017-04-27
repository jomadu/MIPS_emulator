//
//  HazardUnit.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "HazardUnit.hpp"
#include "IFID_PR.hpp"
#include "IDEX_PR.hpp"

HazardUnit::HazardUnit(){
    stall = false;
}

void HazardUnit::update(IFID_PR ifid_pr, IDEX_PR idex_pr){
    unsigned int regFileWriteReg;
    bool ifidIsBranchInstr;
    
    // normal stall case for a load - use hazard
    if ((idex_pr.memRead) && ((idex_pr.instr.rt == ifid_pr.instr.rs) || (idex_pr.instr.rt == ifid_pr.instr.rt))){
        stall = true;
    }
    else{
        stall = false;
    }
    
    //for branches
    // Lets figure out what we know about the preceeding instruction
    // if the regFileWriteReg of the instruction preceeding the branch instruction is the same as the branch instruction's rs or rt, we will need to stall
    if (idex_pr.regDst){
        regFileWriteReg = idex_pr.instr.rd;
    }
    else{
        regFileWriteReg = idex_pr.instr.rt;
    }
    ifidIsBranchInstr = (!ifid_pr.instr.type.compare(BEQ)); // Add more types of branch instructions here
    if (ifidIsBranchInstr && ((regFileWriteReg == ifid_pr.instr.rs) || (regFileWriteReg == ifid_pr.instr.rt))){
        stall = true;
    }
    else{
        stall = false;
    }
    return;
}
