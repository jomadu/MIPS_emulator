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

void ForwardingUnit::updateSTD(IDEX_PR idex, EXMEM_PR exmem, MEMWB_PR memwb){
    // Note: May be a case where mem hazard forwarding overwrites an exection hazard forwarding code
    forwardA = 0x0;
    forwardB = 0x0;
    
    // Execution Hazard Forwarding Detection
    if ((exmem.regWrite) && (exmem.regFileWriteReg != 0) && (exmem.regFileWriteReg == idex.instr.rs)){
        forwardA = 0x2;
    }
    if ((exmem.regWrite) && (exmem.regFileWriteReg != 0) && (exmem.regFileWriteReg == idex.instr.rt)){
        forwardB = 0x2;
    }
    
    // Memory Hazard Forwarding Detection
    if ((memwb.regWrite) && (memwb.regFileWriteReg != 0) && (memwb.regFileWriteReg == idex.instr.rs)){
        forwardA = 0x1;
    }
    if ((memwb.regWrite) && (memwb.regFileWriteReg != 0) && (memwb.regFileWriteReg == idex.instr.rt)){
        forwardB = 0x1;
    }
}
void ForwardingUnit::updateBranching(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb){
    bool ifidIsRTypeBranchInstr = false;
    bool ifidIsITypeBranchInstr = false;
    
    // Figure out what the potential branch instruction is comparing
    if ((!ifid.instr.type.compare(BEQ)) ||
        (!ifid.instr.type.compare(BNE))){
        ifidIsRTypeBranchInstr = true;
    }
    else if ((!ifid.instr.type.compare(BLEZ)) ||
             (!ifid.instr.type.compare(BGTZ)) ||
             (!ifid.instr.type.compare(BLTZ))){
        ifidIsITypeBranchInstr = true;
    }
    
    // Seperate out by I-Type and R-Type
    if (ifidIsRTypeBranchInstr){
        //forwarding?
        // which register to forward?
        if (exmem.regWrite && (ifid.instr.rs == exmem.regFileWriteReg)){
            branchForwardA = 0x2;
            if (memwb.regWrite && (ifid.instr.rt == memwb.regFileWriteReg)){
                branchForwardB = 0x3;
            }
            else{
                branchForwardB = 0x1;
            }
        }
        else if (exmem.regWrite && (ifid.instr.rt == exmem.regFileWriteReg)){
            branchForwardA = 0x2;
            if (memwb.regWrite && (ifid.instr.rs == memwb.regFileWriteReg)){
                branchForwardB = 0x3;
            }
            else{
                branchForwardB = 0x0;
            }
        }
        else if (memwb.regWrite && (ifid.instr.rs == memwb.regFileWriteReg)){
            branchForwardA = 0x3;
            if (exmem.regWrite && (ifid.instr.rt == exmem.regFileWriteReg)){
                branchForwardB = 0x2;
            }
            else{
                branchForwardB = 0x1;
            }
        }
        else if (memwb.regWrite && (ifid.instr.rt == memwb.regFileWriteReg)){
            branchForwardA = 0x3;
            if (exmem.regWrite && (ifid.instr.rs == exmem.regFileWriteReg)){
                branchForwardB = 0x2;
            }
            else{
                branchForwardB = 0x0;
            }
        }
        else{
            branchForwardA = 0x0;
            branchForwardB = 0x1;
        }
    }
    else if(ifidIsITypeBranchInstr){
        if (exmem.regWrite && (ifid.instr.rs == exmem.regFileWriteReg)){
            branchForwardA = 0x2;
        }
        else if (memwb.regWrite && (ifid.instr.rs == memwb.regFileWriteReg)){
            branchForwardA = 0x3;
        }
        else{
            branchForwardA = 0x0;
        }
    }
    else{
        // Should never hit
        branchForwardA = 0x0;
        branchForwardB = 0x1;
    }
}
void ForwardingUnit::updateJumping(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb){
    
}

