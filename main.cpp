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
#include "MIPSStructures.hpp"

using namespace std;

#define R   "R"
#define LW  "LW"
#define LB "LB"
#define LH "LH"
#define LHU  "LHU"
#define LBU  "LBU"
#define SW  "SW"
#define SH "SH"
#define SB  "SB"
#define BEQ "BEQ"
#define BNE "BNE"
#define BGTZ "BGTZ"
#define BLTZ "BLTZ"
#define BLEZ "BLEZ"
#define J   "J"
#define I   "I"
#define MEMORYFILENAME "Regression-Testing/bgtz_test.txt"


// Memory
Memory memory;

//
Cache icache;
Cache dcache;

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
    printf("|------------------------|--------------------------------|---------------------------------|---------------------------------|\n\n");
}

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
            myInstr.type = R;
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
        default:
            myInstr.type = I;
            break;
    }
    
    return myInstr;
}

//  LoadPR
void loadPR(){
    if (!dcache.inPenalty){
        if (hazardUnit.stall){
            ifid_buff.instr.toNOP();
        }
        ifid = ifid_buff;
        idex = idex_buff;
        exmem = exmem_buff;
        memwb = memwb_buff;
    }
}

void IF(){
    bool takeBranch;
    bool regFileReadDataCompare;
    bool branchInstrInID;
    unsigned int branchTarget;
    unsigned int iCacheData;
    
    
    //Computing Branch Target and Branch indicator (PCSrc)
    if (ifid.instr.immed >= 0x8000){
        branchTarget = ((ifid.instr.immed | 0xFFFF0000) << 2) + ifid.pcnext;
    }
    else{
        branchTarget = ((ifid.instr.immed) << 2) + ifid.pcnext;
    }
    
    // Is the branch in ID taken? (PCSrc = true (taken), false (not-taken))
	if (!ifid.instr.type.compare(BEQ)) {
		regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) == regFile.readReg(ifid.instr.rt));
	}
	else if (!ifid.instr.type.compare(BNE)) {
		regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) != regFile.readReg(ifid.instr.rt));
	}
	else if (!ifid.instr.type.compare(BLEZ)) {
		regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) <= 0);
	}
	else if (!ifid.instr.type.compare(BGTZ)) {
		regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) > 0);
	}
	else if (!ifid.instr.type.compare(BLTZ)) {
		regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) < 0);
	}
	else {
		regFileReadDataCompare = false;
	}
    //regFileReadDataCompare = (regFile.readReg(ifid.instr.rs) == regFile.readReg(ifid.instr.rt));
    branchInstrInID = (!ifid.instr.type.compare(BEQ)) || (!ifid.instr.type.compare(BNE)) || (!ifid.instr.type.compare(BLEZ)) || (!ifid.instr.type.compare(BGTZ)) || (!ifid.instr.type.compare(BLTZ)); // Compare returns 0 if strings are equal
    takeBranch = (branchInstrInID && regFileReadDataCompare);
    
    // PC input Mux
    if (takeBranch){
        // Branch in ID taken. Next PC is the branch target
        if (!hazardUnit.stall){
            PC = branchTarget;
        }
    }
    else{
        if (!hazardUnit.stall){
            PC = ifid.pcnext;
        }
    }
    
    // Fetch next instruction from PC address in iCache
    // ifid_buff.instr = decode(memory.fetch(PC));
    iCacheData = icache.loadW(PC, memory);
    
    // Nominal operation
    if (!icache.inPenalty){
        ifid_buff.pcnext = PC + 4;
        ifid_buff.instr = decode(iCacheData);
    }
    else{
        ifid_buff.pcnext = PC;
        ifid_buff.instr.toNOP();
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
    else if (!ifid.instr.type.compare(J)){
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
    
    forwardingUnit.update(idex, exmem, memwb);
    
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
        // R-type
        if (idex.instr.opcode == 0x0){
            switch(idex.instr.funct){
                //goes in order of green cheat sheet
                case 0x20:
                    //add
                    ALUControl = 0x2;
                    break;
                case 0x24:
                    //and
                    ALUControl = 0x0;
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
                case 0x1A:
                    //div
                    ALUControl = 0x5;
                    break;
                case 0x1B:
                    //divu
                    unsignedFlag = true;
                    ALUControl = 0x5;
                    break;
                case 0x18:
                    //mult
                    ALUControl = 0x8;
                case 0x19:
                    //multu
                    unsignedFlag = true;
                    ALUControl = 0x8;
                    break;
                default:
                    printf("Unknown R-type instruction with funct: 0x%X\nDefaulting to sll...\n", idex.instr.funct);
                    ALUControl = 0x4;
                    break;
            }
        }
        else{
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
                    ALUControl = 0x0;
                    break;
                case 0xD:
                    //ori
                    ALUControl = 0x1;
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
        ALUControl = 0x6;
    }
    else{
        printf("Unable to determine ALU control lines.");
        ALUControl = 0x4;
    }
    
    // ALUControl Lines
    // and: 0x0 = 0b000000
    // or : 0x1 = 0b000001
    // add: 0x2 = 0b000010
    // srl: 0x3 = 0b000011
    // sll: 0x4 = 0b000100
    // ?  : 0x5 = 0b000101 - for now it will be for div
    // sub: 0x6 = 0b000110
    // slt: 0x7 = 0b000111
    // ?  : 0x8 = 0b001000 - for now it will be for mult
    // ?  : 0x9 = 0b001001
    // ?  : 0xA = 0b001010
    // ?  : 0xB = 0b001011
    // nor: 0xC = 0b001100
    // ?  : 0xD = 0b001101
    // ?  : 0xE = 0b001110
    // ?  : 0xF = 0b001111
    // .
    // .
    // .
    
    switch (ALUControl) {
        case 0x0:
            // Bitwise AND
            exmem_buff.ALUResult = ALUInput1 & ALUInput2;
            break;
        case 0x1:
            // Bitwise OR
            exmem_buff.ALUResult = ALUInput1 | ALUInput2;
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
        case 0x5:
            // DIV - Apparently we don't have to implement DIV?
            if (unsignedFlag){
                exmem_buff.ALUResult = ALUInput1U / ALUInput2U;
            }
            else{
                exmem_buff.ALUResult = ALUInput1 / ALUInput2;
            }
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
        case 0x8:
            // MULT - Apparently we don't have to implement MULT?
            if (unsignedFlag){
                exmem_buff.ALUResult = ALUInput1U * ALUInput2U;
            }
            else{
                exmem_buff.ALUResult = ALUInput1 * ALUInput2;
            }
            break;
        case 0xC:
            // Bitwise NOR
            exmem_buff.ALUResult = ~(ALUInput1 | ALUInput2);
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

void executeClockCycle(){
    if (debug){
        printf("Executing Cycle...\n\n");
    }
    IF();
    WB();
    ID();
    EX();
    MEM();
    loadPR();
    
    if (debug){
        printf("|------------------------------|\n"
               "| State After Cycle Execution  |\n"
               "|------------------------------|\n\n");
        icache.print();
        dcache.print();
        regFile.print();
        printPipeline();
//        ifid.print();
//        idex.print();
//        exmem.print();
//        memwb.print();
//        regFile.print();
    }
}

void startup(){
    char file [] = MEMORYFILENAME;
    memory.importFile(file, PC);
    ifid.pcnext = PC;
    icache = Cache(16, 1, 2, memory, "iCache");
    dcache = Cache(16, 1, 2, memory, "dCache");
}
int main(int argc, const char * argv[]) {
    startup();
    for (int i = 0; i < 28; i++){
        executeClockCycle();
    }
    return 0;
}

