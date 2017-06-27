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
#include "Constants.hpp"

ForwardingUnit::ForwardingUnit(){
    forwardA = 0x0;
    forwardB = 0x0;
    branchForwardA = 0x0;
    branchForwardB = 0x0;
    jumpForward = 0x0;
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
    if ((memwb.regWrite) &&
        (memwb.regFileWriteReg != 0) &&
        //!(exmem.regWrite && (exmem.regFileWriteReg != 0) && (exmem.regFileWriteReg != idex.instr.rt)) &&
        (memwb.regFileWriteReg == idex.instr.rs))
    {
        forwardA = 0x1;
    }
    if ((memwb.regWrite) &&
        (memwb.regFileWriteReg != 0) &&
        //!(exmem.regWrite && (exmem.regFileWriteReg != 0) && (exmem.regFileWriteReg != idex.instr.rt)) &&
        (memwb.regFileWriteReg == idex.instr.rt)){
        forwardB = 0x1;
    }
    return;
}
void ForwardingUnit::updateBranching(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb){
    // branchForwardA -> branchCondArg1
    // 0x0 -> regFile(ifid.rs)
    // 0x1 -> regFile(ifid.rt)
    // 0x2 -> exmem.ALUResult
    // 0x3 -> memwb.memBypassData
    //
    // branchForwardB -> branchCondArg2
    // 0x0 -> regFile(ifid.rs)
    // 0x1 -> regFile(ifid.rt)
    // 0x2 -> exmem.ALUResult
    // 0x3 -> memwb.memBypassData
    
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
    return;
}
void ForwardingUnit::updateJumping(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb){
    
    // jumpForward -> jumpTargetArg
    // 0x0 -> ifid.instr.addr
    // 0x1 -> regFile(ifid.rs)
    // 0x2 -> exmem.ALUResult
    // 0x3 -> memwb.memBypassData
    
    bool ifidIsRTypeJumpInstr = false;
    bool ifidIsJTypeJumpInstr = false;
    
    // Figure out which type of branch instr we have!
    if (!ifid.instr.type.compare(J) || !ifid.instr.type.compare(JAL)){
        ifidIsJTypeJumpInstr = true;
    }
    else if (!ifid.instr.type.compare(JR)){
        ifidIsRTypeJumpInstr = true;
    }
    
    if (ifidIsJTypeJumpInstr){
        jumpForward = 0x0;
    }
    else if (ifidIsRTypeJumpInstr){
        if (exmem.regWrite && (ifid.instr.rs == exmem.regFileWriteReg)){
            jumpForward = 0x2;
        }
        else if (memwb.regWrite && (ifid.instr.rs == memwb.regFileWriteReg)){
            jumpForward = 0x3;
        }
        else{
            jumpForward = 0x1;
        }
    }
    else{
        jumpForward = 0x0;
    }
    
    return;
}

