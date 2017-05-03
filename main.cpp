
//
//  main.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 2/16/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "MIPSStructures.hpp"
#include "Constants.hpp"

using namespace std;


// Memory
Memory memory;

// Cache
Cache icache;
Cache dcache;
bool icachePrev_inPenalty = false;
bool dcachePrev_inPenalty = false;

// Program Counter
unsigned int PC = 0x0;

// Pipeline Registers
IFID_PR ifid;
IDEX_PR idex;
EXMEM_PR exmem;
MEMWB_PR memwb;

IFID_PR ifid_buff;
IDEX_PR idex_buff;
EXMEM_PR exmem_buff;
MEMWB_PR memwb_buff;

// Hazard Unit
HazardUnit hazardUnit;

// Forwarding Unit
ForwardingUnit forwardingUnit;

// Register File
RegisterFile regFile;

bool branchFlag;
bool jumpFlag;
int savedBranchTarget;
int savedJumpTarget;

int cycleCounter = 0;

Instruction decode(unsigned int mc){
    Instruction myInstr;
    
    unsigned int opcodeMask = 0xFC000000;
    unsigned int rsMask = 0x3E00000;
    unsigned int rtMask = 0x1F0000;
    unsigned int rdMask = 0xF800;
    unsigned int shamtMask = 0x7C0;
    unsigned int functMask = 0x3F;
    unsigned int immedMask = 0xFFFF;
    unsigned int addrMask = 0x3FFFFFF;
    
    unsigned int opcodeShift = 26;
    unsigned int rsShift = 21;
    unsigned int rtShift = 16;
    unsigned int rdShift = 11;
    unsigned int shamtShift = 6;
    
    myInstr.opcode = (mc & opcodeMask) >> opcodeShift;
    myInstr.rs = (mc & rsMask) >> rsShift;
    myInstr.rt = (mc & rtMask) >> rtShift;
    myInstr.rd = (mc & rdMask) >> rdShift;
    myInstr.shamt = (mc & shamtMask) >> shamtShift;
    myInstr.funct = (mc & functMask);
    myInstr.immed = (mc & immedMask);
    myInstr.addr = (mc & addrMask);
    
    switch (myInstr.opcode) {
        case 0x0:
            // Typically R type, butt...
            // JR is weird. We need a case statement here.
            if (myInstr.funct == 0x8){
                myInstr.type = JR;
            }
            else{
                myInstr.type = R;
            }
            break;
        case 0x2:
            myInstr.type = J;
            break;
        case 0x3:
            myInstr.type = JAL;
            break;
        case 0x20:
            myInstr.type = LB;
            break;
        case 0x21:
            myInstr.type = LH;
            break;
        case 0x23:
            myInstr.type = LW;
            break;
        case 0x24:
            myInstr.type = LBU;
            break;
        case 0x25:
            myInstr.type = LHU;
            break;
        case 0x28:
            myInstr.type = SB;
            break;
        case 0x29:
            myInstr.type = SH;
            break;
        case 0x2B:
            myInstr.type = SW;
            break;
        case 0x4:
            myInstr.type = BEQ;
            break;
		case 0x5:
			myInstr.type = BNE;
			break;
		case 0x6:
			myInstr.type = BLEZ;
			break;
		case 0x7:
			myInstr.type = BGTZ;
			break;
		case 0x1:
			myInstr.type = BLTZ;
			break;
        case 0x1F:
            myInstr.type = R;
            break;
        default:
            myInstr.type = I;
            break;
    }
    
    return myInstr;
}

void IF(){
    bool takeBranch;
    bool regFileReadDataBranchCondMet = false;
    bool branchInstrInIFID = false;
    bool jumpInstrInIFID = false;
    int branchTarget;
    int jumpTarget;
    unsigned int iCacheData;
    int branchCondArg1;
    int branchCondArg2;
    int jumpTargetArg;
    
    forwardingUnit.updateBranching(ifid, exmem, memwb);
    forwardingUnit.updateJumping(ifid, exmem, memwb);
    
    // Compute the branch target from the sign extended immed field.
    if (ifid.instr.immed >= 0x8000){
        branchTarget = ((ifid.instr.immed | 0xFFFF0000) << 2) + ifid.pcnext;
    }
    else{
        branchTarget = ((ifid.instr.immed) << 2) + ifid.pcnext;
    }
    
    // Determines BCA1
    switch(forwardingUnit.branchForwardA){
        case 0x0:
            branchCondArg1 = regFile.readReg(ifid.instr.rs);
            break;
        case 0x1:
            branchCondArg1 = regFile.readReg(ifid.instr.rt);
            break;
        case 0x2:
            branchCondArg1 = exmem.ALUResult;
            break;
        case 0x3:
            branchCondArg1 = memwb.memBypassData;
            break;
        default:
            branchCondArg1 = regFile.readReg(ifid.instr.rs);
            break;
    }
    // Determines BCA2
    switch(forwardingUnit.branchForwardB){
        case 0x0:
            branchCondArg2 = regFile.readReg(ifid.instr.rs);
            break;
        case 0x1:
            branchCondArg2 = regFile.readReg(ifid.instr.rt);
            break;
        case 0x2:
            branchCondArg2 = exmem.ALUResult;
            break;
        case 0x3:
            branchCondArg2 = memwb.memBypassData;
            break;
        default:
            branchCondArg2 = regFile.readReg(ifid.instr.rs);
            break;
    }
    
    // Evaluate the branch condition and if (branch in ifid)
    branchInstrInIFID = true;
	if (!ifid.instr.type.compare(BEQ)) {
		regFileReadDataBranchCondMet = (branchCondArg1 == branchCondArg2);
	}
	else if (!ifid.instr.type.compare(BNE)) {
		regFileReadDataBranchCondMet = (branchCondArg1 != branchCondArg2);
	}
	else if (!ifid.instr.type.compare(BLEZ)) {
		regFileReadDataBranchCondMet = (branchCondArg1 <= 0);
	}
	else if (!ifid.instr.type.compare(BGTZ)) {
		regFileReadDataBranchCondMet = (branchCondArg1 > 0);
	}
	else if (!ifid.instr.type.compare(BLTZ)) {
		regFileReadDataBranchCondMet = (branchCondArg1 < 0);
	}
	else {
		regFileReadDataBranchCondMet = false;
        branchInstrInIFID = false;
	}
    
    // Computing Branch indicator (takeBranch)
    takeBranch = (branchInstrInIFID && regFileReadDataBranchCondMet);
    
    if (branchInstrInIFID && takeBranch){
        branchFlag = true;
        savedBranchTarget = branchTarget;
    }
    
    // Jump Stuff
    
    // Determines JTA and Jump target
    switch(forwardingUnit.jumpForward){
        case 0x0:
            jumpTargetArg = ifid.instr.addr;
            jumpTarget = (ifid.pcnext & 0xF0000000) | (jumpTargetArg << 2);
            break;
        case 0x1:
            jumpTargetArg = regFile.readReg(ifid.instr.rs);
            jumpTarget = jumpTargetArg;
            break;
        case 0x2:
            jumpTargetArg = exmem.ALUResult;
            jumpTarget = jumpTargetArg;
            break;
        case 0x3:
            jumpTargetArg = memwb.memBypassData;
            jumpTarget = jumpTargetArg;
            break;
        default:
            jumpTargetArg = ifid.instr.addr;
            jumpTarget = (ifid.pcnext & 0xF0000000) | (jumpTargetArg << 2);
            break;
    }
    
    // Determine if there is a jump instr in the ifid field
    jumpInstrInIFID = (!ifid.instr.type.compare(J) || !ifid.instr.type.compare(JAL) || !ifid.instr.type.compare(JR));
    
    if (jumpInstrInIFID){
        jumpFlag = true;
        savedJumpTarget = jumpTarget;
    }
    
    // PC input Mux
    if(hazardUnit.stall){
        PC = PC;
    }
    else{
        // PC stays the same
        PC = ifid.pcnext;
    }
    
    // Fetch next instruction from PC address in iCache
    // ifid_buff.instr = decode(memory.fetch(PC));
    iCacheData = icache.loadW(PC, memory);
    
    bool icacheInPenalty = icache.inPenalty;
    bool dcacheInPenalty = dcache.inPenalty && (dcache.penaltyCounter != 0);
    
    ifid_buff.instr = decode(iCacheData);

    if (icacheInPenalty && dcacheInPenalty){
        ifid_buff.instr.toNOP();
        ifid_buff.pcnext = PC;
    }
    else if (icacheInPenalty && !dcacheInPenalty){
        ifid_buff.instr.toNOP();
        ifid_buff.pcnext = PC;
    }
    else if (!icacheInPenalty && dcacheInPenalty){
        ifid_buff.pcnext = PC;
    }
    else if (!icacheInPenalty && !dcacheInPenalty){
        if (branchFlag){
            ifid_buff.pcnext = savedBranchTarget;
            branchFlag = false;
        }
        else if (jumpFlag){
            ifid_buff.pcnext = savedJumpTarget;
            jumpFlag = false;
        }
        else{
            ifid_buff.pcnext = PC + 4;
        }
        
    }
    
    if (!ifid_buff.instr.type.compare(JAL)){
        regFile.writeReg(31, PC + 8);
    }
    
    
}

void WB(){
    if (memwb.regWrite){
        if (memwb.memToReg){
            regFile.writeReg(memwb.regFileWriteReg, memwb.memReadData);
        }
        else{
            regFile.writeReg(memwb.regFileWriteReg, memwb.memBypassData);
        }
    }
}

void ID(){
    idex_buff.instr = ifid.instr;
    
    if (!ifid.instr.type.compare(R)){
        idex_buff.regDst = true;
        idex_buff.ALUOp0 = false;
        idex_buff.ALUOp1 = true;
        idex_buff.ALUSrc = false;
        idex_buff.branch = false;
        idex_buff.memRead = false;
        idex_buff.memWrite = false;
        idex_buff.regWrite = true;
        idex_buff.memToReg = false;
    }
    else if (!ifid.instr.type.compare(LW) || !ifid.instr.type.compare(LB) || !ifid.instr.type.compare(LH) || !ifid.instr.type.compare(LBU) || !ifid.instr.type.compare(LHU)){
        idex_buff.regDst = false;
        idex_buff.ALUOp0 = false;
        idex_buff.ALUOp1 = false;
        idex_buff.ALUSrc = true;
        idex_buff.branch = false;
        idex_buff.memRead = true;
        idex_buff.memWrite = false;
        idex_buff.regWrite = true;
        idex_buff.memToReg = true;
    }
    else if (!ifid.instr.type.compare(SW) || !ifid.instr.type.compare(SH) || !ifid.instr.type.compare(SB)){
        idex_buff.regDst = false;
        idex_buff.ALUOp0 = false;
        idex_buff.ALUOp1 = false;
        idex_buff.ALUSrc = true;
        idex_buff.branch = false;
        idex_buff.memRead = false;
        idex_buff.memWrite = true;
        idex_buff.regWrite = false;
        idex_buff.memToReg = false;
    }
    else if (!ifid.instr.type.compare(BEQ) || !ifid.instr.type.compare(BNE) || !ifid.instr.type.compare(BLEZ) || !ifid.instr.type.compare(BGTZ) || !ifid.instr.type.compare(BLTZ)){
        idex_buff.regDst = false;
        idex_buff.ALUOp0 = true;
        idex_buff.ALUOp1 = false;
        idex_buff.ALUSrc = false;
        idex_buff.branch = true;
        idex_buff.memRead = false;
        idex_buff.memWrite = false;
        idex_buff.regWrite = false;
        idex_buff.memToReg = false;
    }
    else if (!ifid.instr.type.compare(J) || !ifid.instr.type.compare(JAL) || !ifid.instr.type.compare(JR)){
        idex_buff.regDst = false;
        idex_buff.ALUOp0 = false;
        idex_buff.ALUOp1 = false;
        idex_buff.ALUSrc = false;
        idex_buff.branch = false;
        idex_buff.memRead = false;
        idex_buff.memWrite = false;
        idex_buff.regWrite = false;
        idex_buff.memToReg = false;
    }
    else if (!ifid.instr.type.compare(I)){
        idex_buff.regDst = false;
        idex_buff.ALUOp0 = false;
        idex_buff.ALUOp1 = true;
        idex_buff.ALUSrc = true;
        idex_buff.branch = false;
        idex_buff.memRead = false;
        idex_buff.memWrite = false;
        idex_buff.regWrite = true;
        idex_buff.memToReg = false;
    }
    else{
        printf("ID: Unable to derive control lines from instruction.\n\n");
    }

    idex_buff.regFileReadData1 = regFile.readReg(ifid.instr.rs);
    idex_buff.regFileReadData2 = regFile.readReg(ifid.instr.rt);
    idex_buff.pcnext = ifid.pcnext;
    
    if (ifid.instr.immed >= 0x8000){
        idex_buff.signExtend = ifid.instr.immed + 0xFFFF0000;
    }
    else{
        idex_buff.signExtend = ifid.instr.immed;
    }
    
    idex_buff.zeroExtend = ifid.instr.immed;
    
    hazardUnit.update(ifid_buff, idex_buff);
    
}

void EX(){
    unsigned int ALUInput1U;
    unsigned int ALUInput2U;
    int ALUInput1;
    int ALUInput2;
    unsigned int ALUInput1ForwardingMux;
    unsigned int ALUInput2ForwardingMux;
    unsigned int ALUControl;
    unsigned int shamt;
    unsigned int shamtMask = 0x3E0;
    bool unsignedFlag = false;
    
    exmem_buff.regWrite = idex.regWrite;
    exmem_buff.memToReg = idex.memToReg;
    exmem_buff.memRead = idex.memRead;
    exmem_buff.memWrite = idex.memWrite;
    
    exmem_buff.instr = idex.instr;
    
    forwardingUnit.updateSTD(idex, exmem, memwb);
    
    // Determine ALUInputs
    // ALUInput1 Forwarding Mux
    switch (forwardingUnit.forwardA) {
        case 0x0:
            // No Forwarding needed
            ALUInput1ForwardingMux = idex.regFileReadData1;
            break;
        case 0x1:
            // Forwarding from memwb
            if (memwb.memToReg){
                ALUInput1ForwardingMux = memwb.memReadData;
            }
            else{
                ALUInput1ForwardingMux = memwb.memBypassData;
            }
            break;
        case 0x2:
            // Forwarding from exmem
            ALUInput1ForwardingMux = exmem.ALUResult;
            break;
        default:
            perror("ForwardA was a weird value.");
            ALUInput1ForwardingMux = idex.regFileReadData1;
            break;
    }
    ALUInput1U = ALUInput1ForwardingMux;
    ALUInput1 = (int) ALUInput1U;
    
    //ALUInput2 Forwarding Mux
    switch (forwardingUnit.forwardB) {
        case 0x0:
            // No Forwarding needed
            ALUInput2ForwardingMux = idex.regFileReadData2;
            break;
        case 0x1:
            // Forwarding from memwb
            if (memwb.memToReg){
                ALUInput2ForwardingMux = memwb.memReadData;
            }
            else{
                ALUInput2ForwardingMux = memwb.memBypassData;
            }
            break;
        case 0x2:
            // Forwarding from exmem
            ALUInput2ForwardingMux = exmem.ALUResult;
            break;
        default:
            perror("ForwardB was a weird value.");
            ALUInput2ForwardingMux = idex.regFileReadData2;
            break;
    }
    
    if (idex.ALUSrc){
        ALUInput2U = idex.signExtend;
        ALUInput2 = (int) ALUInput2U;
    }
    else{
        ALUInput2U = ALUInput2ForwardingMux;
        ALUInput2 = (int) ALUInput2U;
    }
    
    // Determine ALU Controls
    if (idex.ALUOp1 && !idex.ALUOp0){
        // R-type or I-Type instructions
        if (!idex.instr.type.compare(R)){
            // R-type
            switch(idex.instr.funct){
                //goes in order of green cheat sheet
                case 0x20:
                    if (idex.instr.opcode == 0x1F){
                        //seb
                        ALUControl = 0x11;
                    }
                    else{
                        //add
                        ALUControl = 0x2;
                    }
                    break;
                case 0x21:
                    //addu
                    unsignedFlag = true;
                    ALUControl = 0x2;
                    break;
                case 0x24:
                    //and
                    ALUControl = 0x0;
                    break;
                case 0x26:
                    //xor
                    ALUControl = 0xF;
                    break;
                case 0x27:
                    //nor
                    ALUControl = 0xC;
                    break;
                case 0x25:
                    //or
                    ALUControl = 0x1;
                    break;
                case 0x2A:
                    //slt
                    ALUControl = 0x7;
                    break;
                case 0x2B:
                    //sltu
                    unsignedFlag = true;
                    ALUControl = 0x7;
                    break;
                case 0x0:
                    //sll
                    ALUControl = 0x4;
                    break;
                case 0x2:
                    //srl
                    ALUControl = 0x3;
                    break;
                case 0x22:
                    //sub
                    ALUControl = 0x6;
                case 0x23:
                    //subu
                    unsignedFlag = true;
                    ALUControl = 0x6;
                    break;
                case 0xA:
                    //movz
                    ALUControl = 0xD;
                    break;
                case 0xB:
                    //movn
                    ALUControl = 0xE;
                    break;
                default:
                    printf("Unknown R-type instruction with funct: 0x%X\nDefaulting to sll...\n", idex.instr.funct);
                    ALUControl = 0x4;
                    break;
            }
        }
        else{
            // I-Type
            switch(idex.instr.opcode){
                case 0x8:
                    //addi
                    ALUControl = 0x2;
                    break;
                case 0x9:
                    //addiu
                    unsignedFlag = true;
                    ALUControl = 0x2;
                    break;
                case 0xC:
                    //andi
                    //since we have a bitwise immediate instr, we need ALUInput2 to be the zero-extended
                    ALUControl = 0x0;
                    ALUInput2U = idex.zeroExtend;
                    ALUInput2 = (int) ALUInput2U;
                    break;
                case 0xD:
                    //ori
                    //since we have a bitwise immediate instr, we need ALUInput2 to be the zero-extended
                    ALUControl = 0x1;
                    ALUInput2U = idex.zeroExtend;
                    ALUInput2 = (int) ALUInput2U;
                    break;
                case 0xA:
                    //slti
                    ALUControl = 0x7;
                    break;
                case 0xB:
                    //sltiu
                    unsignedFlag = true;
                    ALUControl = 0x7;
                    break;
                case 0xE:
                    //xori
                    //since we have a bitwise immediate instr, we need ALUInput2 to be the zero-extended
                    ALUControl = 0xF;
                    break;
                case 0xF:
                    //lui
                    ALUControl = 0x10;
                    break;
                default:
                    printf("Unknown I-type instruction with opcode: 0x%X\nDefaulting to sll...\n", idex.instr.opcode);
                    ALUControl = 0x4;
                    break;
            }
        }
        
    }
    else if (!idex.ALUOp1 && !idex.ALUOp0){
        // LW and SW instructions
        ALUControl = 0x2;
    }
    else if (!idex.ALUOp1 && idex.ALUOp0){
        // BEQ instructions
        ALUControl = 0x4;
    }
    else{
        printf("Unable to determine ALU control lines.");
        ALUControl = 0x4;
    }
    
    switch (ALUControl) {
        case 0x0:
            // Bitwise AND
            exmem_buff.ALUResult = ALUInput1U & ALUInput2U;
            break;
        case 0x1:
            // Bitwise OR
            exmem_buff.ALUResult = ALUInput1U | ALUInput2U;
            break;
        case 0x2:
            // ADD
            if (unsignedFlag){
                exmem_buff.ALUResult = ALUInput1U + ALUInput2U;
            }
            else{
                exmem_buff.ALUResult = ALUInput1 + ALUInput2;
            }
            break;
        case 0x3:
            // SRL
            // TODO: May need to consider signed/unsigned int
            shamt = (idex.signExtend & shamtMask) >> 6;
            exmem_buff.ALUResult = ALUInput2 >> shamt;
            break;
        case 0x4:
            // SLL
            // TODO: May need to consider signed/unsigned int
            shamt = (idex.signExtend & shamtMask) >> 6;
            exmem_buff.ALUResult = ALUInput2 << shamt;
            break;
        case 0x6:
            // SUB
            if (unsignedFlag){
                exmem_buff.ALUResult = ALUInput1U - ALUInput2U;
            }
            else{
                exmem_buff.ALUResult = ALUInput1 - ALUInput2;
            }
            break;
        case 0x7:
            // SLT
            if (unsignedFlag){
                if (ALUInput1U < ALUInput2U){
                    exmem_buff.ALUResult = 0x1;
                }
                else{
                    exmem_buff.ALUResult = 0x0;
                }
            }
            else{
                if (ALUInput1 < ALUInput2){
                    exmem_buff.ALUResult = 0x1;
                }
                else{
                    exmem_buff.ALUResult = 0x0;
                }
            }
            
            break;
        case 0xC:
            // Bitwise NOR
            exmem_buff.ALUResult = ~(ALUInput1U | ALUInput2U);
            break;
        case 0xD:
            // movz
            // moves rs to rd if rt = 0
            exmem_buff.ALUResult = ALUInput1;
            if (ALUInput2 != 0){
                exmem_buff.regWrite = false;
            }
            break;
        case 0xE:
            // movn
            // moves rs to rd if rt != 0
            exmem_buff.ALUResult = ALUInput1;
            if (ALUInput2 == 0){
                exmem_buff.regWrite = false;
            }
            break;
        case 0xF:
            //BITWISE xor
            exmem_buff.ALUResult = (ALUInput1U ^ ALUInput2U);
            break;
        case 0x10:
            //lui
            exmem_buff.ALUResult = (ALUInput2 << 16);
            break;
        case 0x11:
            //seb
            if ((ALUInput2U & 0xFF) >= 0x80){
                exmem_buff.ALUResult = (ALUInput2U | 0xFFFFFF00);
            }
            else{
                exmem_buff.ALUResult = (ALUInput2U & 0xFF);
            }
            break;
        default:
            perror("Unknown ALU Controls\n");
            break;
    }
    
    exmem_buff.memWriteData = ALUInput2ForwardingMux;
    
    if (idex.regDst){
        exmem_buff.regFileWriteReg = idex.instr.rd;
    }
    else{
        exmem_buff.regFileWriteReg = idex.instr.rt;
    }
}

void MEM(){
    unsigned int dCacheData;

    memwb_buff.instr = exmem.instr;
    memwb_buff.regWrite = exmem.regWrite;
    memwb_buff.memToReg = exmem.memToReg;
    
    if (exmem.memRead && !exmem.memWrite){
        if (!exmem.instr.type.compare(LBU)){
            dCacheData = dcache.loadBU(exmem.ALUResult, memory);
            memwb_buff.memReadData = dCacheData;
        }
        else if (!exmem.instr.type.compare(LB)){
            dCacheData = dcache.loadB(exmem.ALUResult, memory);
            memwb_buff.memReadData = dCacheData;
        }
        else if (!exmem.instr.type.compare(LHU)){
            dCacheData = dcache.loadHWU(exmem.ALUResult, memory);
            memwb_buff.memReadData = dCacheData;
        }
        else if (!exmem.instr.type.compare(LH)){
            dCacheData = dcache.loadHW(exmem.ALUResult, memory);
            memwb_buff.memReadData = dCacheData;
        }
        else{
            dCacheData = dcache.loadW(exmem.ALUResult, memory);
            memwb_buff.memReadData = dCacheData;
        }
    }
    else if (!exmem.memRead && exmem.memWrite){
        memwb_buff.memReadData = 0x0;
        if (!exmem.instr.type.compare(SB)){
            dcache.storeB(exmem.memWriteData, exmem.ALUResult, memory);
        }
        else if (!exmem.instr.type.compare(SH)){
            dcache.storeHW(exmem.memWriteData, exmem.ALUResult, memory);
        }
        else{
            dcache.storeW(exmem.memWriteData, exmem.ALUResult, memory);
        }
    }
    else if (exmem.memRead && exmem.memWrite){
        perror("memRead and memWrite control lines both high.\n");
    }
    memwb_buff.memBypassData = exmem.ALUResult;
    memwb_buff.regFileWriteReg = exmem.regFileWriteReg;
}

void loadPR(){
    if (!dcache.inPenalty){
        if (hazardUnit.stall){
            ifid_buff.instr.toNOP();
            ifid_buff.pcnext = PC;
        }
        ifid = ifid_buff;
        idex = idex_buff;
        exmem = exmem_buff;
        memwb = memwb_buff;
    }
}

void executeClockCycle(){
    IF();
    WB();
    ID();
    EX();
    MEM();
    loadPR();
}

void printPipeline(){
    printf("|------------------------|--------------------------------|---------------------------------|---------------------------------|\n"
           "|IFID Pipeline Register  |IDEX Pipeline Register          |EXMEM Pipeline Register          |MEMWB Pipeline Register          |\n"
           "|------------------------|--------------------------------|---------------------------------|---------------------------------|\n"
           "|Instruction:            |                                |                                 |                                 |\n"
           "|------------            |------------                    |------------                     |------------                     |\n");
    printf("|opcode: 0x%-14X|opcode: 0x%-22X|opcode: 0x%-23X|opcode: 0x%-23X|\n", ifid.instr.opcode,idex.instr.opcode,exmem.instr.opcode,memwb.instr.opcode);
    printf("|rs:     0x%-14X|rs:     0x%-22X|rs:     0x%-23X|rs:     0x%-23X|\n", ifid.instr.rs,idex.instr.rs,exmem.instr.rs,memwb.instr.rs);
    printf("|rt:     0x%-14X|rt:     0x%-22X|rt:     0x%-23X|rt:     0x%-23X|\n", ifid.instr.rt,idex.instr.rt,exmem.instr.rt,memwb.instr.rt);
    printf("|rd:     0x%-14X|rd:     0x%-22X|rd:     0x%-23X|rd:     0x%-23X|\n", ifid.instr.rd,idex.instr.rd,exmem.instr.rd,memwb.instr.rd);
    printf("|shamt:  0x%-14X|shamt:  0x%-22X|shamt:  0x%-23X|shamt:  0x%-23X|\n", ifid.instr.shamt,idex.instr.shamt,exmem.instr.shamt,memwb.instr.shamt);
    printf("|funct:  0x%-14X|funct:  0x%-22X|funct:  0x%-23X|funct:  0x%-23X|\n", ifid.instr.funct,idex.instr.funct,exmem.instr.funct,memwb.instr.funct);
    printf("|immed:  0x%-14X|immed:  0x%-22X|immed:  0x%-23X|immed:  0x%-23X|\n", ifid.instr.immed,idex.instr.immed,exmem.instr.immed,memwb.instr.immed);
    printf("|addr:   0x%-14X|addr:   0x%-22X|addr:   0x%-23X|addr:   0x%-23X|\n", ifid.instr.addr,idex.instr.addr,exmem.instr.addr,memwb.instr.addr);
    printf("|type:   %-16s|type:   %-24s|type:   %-25s|type:   %-25s|\n", ifid.instr.type.c_str(),idex.instr.type.c_str(),exmem.instr.type.c_str(),memwb.instr.type.c_str());
    printf("|                        |                                |                                 |                                 |\n"
           "|Other                   |                                |                                 |                                 |\n"
           "|------------            |------------                    |------------                     |------------                     |\n");
    printf("|PCnext: 0x%-14X|regWrite:           %-12s|regWrite:           %-13s|regWrite:           %-13s|\n",
           ifid.pcnext,
           idex.regWrite ? "true":"false",
           exmem.regWrite ? "true": "false",
           memwb.regWrite ? "true" : "false");
    printf("|                        |memToReg:           %-12s|memToReg:           %-13s|memToReg:           %-13s|\n",
           idex.memToReg ? "true":"false",
           exmem.memToReg ? "true": "false",
           memwb.memToReg ? "true" : "false");
    printf("|                        |memWrite:           %-12s|memWrite:           %-13s|memReadData:        0x%-11X|\n",
           idex.memWrite ? "true":"false",
           exmem.memWrite ? "true": "false",
           memwb.memReadData);
    printf("|                        |memRead:            %-12s|memRead:            %-13s|memBypassData:      0x%-11X|\n",
           idex.memRead ? "true":"false",
           exmem.memRead ? "true": "false",
           memwb.memBypassData);
    printf("|                        |branch:             %-12s|ALUResult:          0x%-11X|                                 |\n",
           idex.branch ? "true":"false",
           exmem.ALUResult);
    printf("|                        |regDst:             %-12s|memWriteData:       0x%-11X|                                 |\n",
           idex.regDst ? "true":"false",
           exmem.memWriteData);
    printf("|                        |ALUOP0:             %-12s|regFileWriteReg:    0x%-11X|regFileWriteReg:    0x%-11X|\n",
           idex.ALUOp0 ? "true":"false",
           exmem.regFileWriteReg,
           memwb.regFileWriteReg);
    printf("|                        |ALUOP1:             %-12s|                                 |                                 |\n",
           idex.ALUOp1 ? "true":"false");
    printf("|                        |ALUsrc:             %-12s|                                 |                                 |\n",
           idex.ALUSrc ? "true":"false");
    printf("|                        |PCnext:             0x%-10X|                                 |                                 |\n",
           idex.pcnext);
    printf("|                        |regFileReadData1:   0x%-10X|                                 |                                 |\n",
           idex.regFileReadData1);
    printf("|                        |regFileReadData2:   0x%-10X|                                 |                                 |\n",
           idex.regFileReadData2);
    printf("|                        |signExtend:         0x%-10X|                                 |                                 |\n",
           idex.signExtend);
    printf("|                        |zeroExtend:         0x%-10X|                                 |                                 |\n",
           idex.zeroExtend);
    printf("|------------------------|--------------------------------|---------------------------------|---------------------------------|\n\n");
}

void updateCacheHitRatesAndNumInstr(){
    bool loadOrStoreInstrInMEMWBBUFF = false;
    
    if(icachePrev_inPenalty && icache.inPenalty){
        // icache - no access, no numHits
    }
    else if (icachePrev_inPenalty && !icache.inPenalty){
        // icache - no access, no numHits
    }
    else if (!icachePrev_inPenalty && icache.inPenalty){
        // icache - access(dcache.inPenalty), no numHits
        if (!dcache.inPenalty){
            icache.numAccesses++;
        }
    }
    else {
        // icache - Access(dcache.inPenalty), hit(dcache.inPenalty)
        if (!dcache.inPenalty){
            icache.numHits++;
            icache.numAccesses++;
        }
    }
    
    loadOrStoreInstrInMEMWBBUFF = (
                                   !memwb_buff.instr.type.compare(LW) ||
                                   !memwb_buff.instr.type.compare(LH) ||
                                   !memwb_buff.instr.type.compare(LHU) ||
                                   !memwb_buff.instr.type.compare(LB) ||
                                   !memwb_buff.instr.type.compare(LBU) ||
                                   !memwb_buff.instr.type.compare(SW) ||
                                   !memwb_buff.instr.type.compare(SH) ||
                                   !memwb_buff.instr.type.compare(SB)
                                   );
    if (loadOrStoreInstrInMEMWBBUFF){
        // Update dCache
        if(dcachePrev_inPenalty && dcache.inPenalty){
            // dcache - no access, no numHits
        }
        else if (dcachePrev_inPenalty && !dcache.inPenalty){
            // dcache - no access, no numHits
        }
        else if (!dcachePrev_inPenalty && dcache.inPenalty){
            // dcache - Access, no numHits
            dcache.numAccesses++;
        }
        else {
            // dcache - Access(icachePrev_inPenalty, icache.inPenalty), hit(icachePrev_inPenalty, icache.inPenalty)
            dcache.numHits++;
            dcache.numAccesses++;
        }
    }
    
    icache.hitRate = (float) icache.numHits / (float) icache.numAccesses * 100;
    dcache.hitRate = (float) dcache.numHits / (float) dcache.numAccesses * 100;
    
    icachePrev_inPenalty = icache.inPenalty;
    dcachePrev_inPenalty = dcache.inPenalty;
}

void startup(){
    char filePath[80];
    
    if (PROGRAM == 1){
        strcpy(filePath, PROGRAM1);
    }
    else if (PROGRAM == 2){
        strcpy(filePath, PROGRAM2);
    }
    else{
        strcpy(filePath, TESTPROGRAM);
    }
    
    int error = memory.importFile(filePath);
    if (error){
        exit(1);
    }
    icache = Cache(I_CACHE_SIZE, memory, "iCache");
    dcache = Cache(D_CACHE_SIZE, memory, "dCache");
    if (PROGRAM == 1){
        regFile.writeReg(29, memory.loadW(0x0));
        regFile.writeReg(30, memory.loadW(0x4));
        PC = (memory.loadW(0x14) << 2);
    }
    else if (PROGRAM == 2){
        PC = 0x0;
    }
    else{
        PC = 0x0;
    }
    ifid.pcnext = PC;
    
}

void finish(){
    printf("************************************************************\n"
           "*  _______                          __   __                 *\n"
           "* |    ___|.--.--.-----.----.--.--.|  |_|__|.-----.-----.   *\n"
           "* |    ___||_   _|  -__|  __|  |  ||   _|  ||  _  |     |   *\n"
           "* |_______||__.__|_____|____|_____||____|__||_____|__|__|   *\n");
    printf("*  ______                        __         __          __  *\n"
           "* |      |.-----.--------.-----.|  |.-----.|  |_.-----.|  | *\n"
           "* |   ---||  _  |        |  _  ||  ||  -__||   _|  -__||__| *\n"
           "* |______||_____|__|__|__|   __||__||_____||____|_____||__| *\n"
           "*                        |__|                               *\n"
           "*************************************************************\n");
    memory.print(0, 1200);
}

int main(int argc, const char * argv[]) {
    startup();
    while (PC != 0x0){
        if (DEBUG){
            printf("\nExecuting Cycle...\n\n");
        }
        executeClockCycle();
        updateCacheHitRatesAndNumInstr();
        cycleCounter ++;
        if (DEBUG){
            printf("|------------------------------|\n"
                   "| State After Cycle Execution  |\n"
                   "|------------------------------|\n\n");
            icache.print();
            dcache.print();
            regFile.print();
            printPipeline();
            printf("PC: 0x%-4X (%4i) PrgL: %4i\n"
                   "CC: %i\n"
                   "Stall: %-6s\n"
                   "ic.inPen: %-6s\n"
                   "ic.penCntr: %i\n"
                   "ic.hitrate: %-8.5f%%"
                   "dc.inPen: %-6s\n"
                   "dc.penCntr: %i\n"
                   "ic.hitrate: %-8.5f%%",
                   PC,PC,PC/4+1,cycleCounter,
                   hazardUnit.stall ? "true": "false",
                   icache.inPenalty ? "true": "false",
                   icache.penaltyCounter,
                   icache.hitRate,
                   dcache.inPenalty ? "true": "false",
                   dcache.penaltyCounter,
                   dcache.hitRate);
        }
        else{
            printf("| PC: 0x%-4X (%4i) | PrgL: %4i | CC: %i | iCache HR: %-8.5f%% | dCache HR: %-8.5f%% |\n",
                   PC,PC,PC/4+1,cycleCounter,icache.hitRate,dcache.hitRate);
        }
    }
    finish();
    return 0;
}

