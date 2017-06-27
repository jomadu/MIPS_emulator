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
#include "EXMEM_PR.hpp"
#include "Constants.hpp"

HazardUnit::HazardUnit(){
    stall = false;
}

void HazardUnit::update(IFID_PR ifid_buff, IDEX_PR idex_buff, IDEX_PR idex){
    unsigned int idex_buff_regFileWriteReg;
    unsigned int exmem_buff_regFileWriteReg;
    bool ifidBuffIsRTypeBranchInstr = false;
    bool ifidBuffIsITypeBranchInstr = false;
    
    // normal stall case for a load - use hazard
    if ((idex_buff.memRead) && ((idex_buff.instr.rt == ifid_buff.instr.rs) || (idex_buff.instr.rt == ifid_buff.instr.rt))){
        stall = true;
    }
    else{
        stall = false;
    }
    
    // For branches:
    // First, determine whether we have a i-type or r-type branch instruction in the ifid buff
    if ((!ifid_buff.instr.type.compare(BEQ)) ||
        (!ifid_buff.instr.type.compare(BNE))){
        ifidBuffIsRTypeBranchInstr = true;
    }
    else if ((!ifid_buff.instr.type.compare(BLEZ)) ||
             (!ifid_buff.instr.type.compare(BGTZ)) ||
             (!ifid_buff.instr.type.compare(BLTZ))){
        ifidBuffIsITypeBranchInstr = true;
    }
    // Next we need the regFileWriteRegisters from the idex_buff and exmem_buff pr's
    // We know exactly what the idex_buff is, but we need to extrapolate the exmem_buff
    
    if (idex_buff.regDst){
        idex_buff_regFileWriteReg = idex_buff.instr.rd;
    }
    else{
        idex_buff_regFileWriteReg = idex_buff.instr.rt;
    }
    
    if (idex.regDst){
        exmem_buff_regFileWriteReg = idex.instr.rd;
    }
    else{
        exmem_buff_regFileWriteReg = idex.instr.rt;
    }
    
    // Stall Logic
    if (ifidBuffIsRTypeBranchInstr){
        if (((idex_buff_regFileWriteReg != 0) && ((idex_buff_regFileWriteReg == ifid_buff.instr.rs) || (idex_buff_regFileWriteReg == ifid_buff.instr.rt))) ||
            ((exmem_buff_regFileWriteReg != 0) && ((exmem_buff_regFileWriteReg == ifid_buff.instr.rs) || (exmem_buff_regFileWriteReg == ifid_buff.instr.rt)))){
            stall = true;
        }
        else{
            stall = false;
        }
    }
    else if (ifidBuffIsITypeBranchInstr){
        if (((idex_buff_regFileWriteReg != 0) && (idex_buff_regFileWriteReg == ifid_buff.instr.rs)) ||
            ((exmem_buff_regFileWriteReg != 0) && (exmem_buff_regFileWriteReg == ifid_buff.instr.rs))){
            stall = true;
        }
        else{
            stall = false;
        }
    }
             
    return;

}
