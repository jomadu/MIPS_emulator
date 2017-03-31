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

bool debug = true;
#define R   "R"
#define LW  "LW"
#define SW  "SW"
#define BEQ "BEQ"
#define MEMORYFILENAME "memory.txt"

// Memory
Memory memory;

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

Instruction decode(unsigned int mc){
    //  MCDecode(MC)
    //  Input -> MC in 8 digit HEX int
    //  Returns -> Instruction Object
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
    myInstr.shamt = (mc & shamtMask) >> shamtMask;
    myInstr.funct = (mc & functMask);
    myInstr.immed = (mc & immedMask);
    myInstr.addr = (mc & addrMask);
    
    if (myInstr.opcode == 0x0){
        myInstr.type = R;
    }
    else if (myInstr.funct == 0x23){
        myInstr.type = LW;
    }
    else if (myInstr.funct == 0x2B){
        myInstr.type = SW;
    }
    else if (myInstr.funct == 0x4){
        myInstr.type = BEQ;
    }
    
    if (debug){
        printf("Decoding...\n");
        printf("MC: 0x%X\n", mc);
        printf("Instruction:\n"
               "------------\n"
               "opcode:\t0x%X\n"
               "rs:\t\t0x%X\n"
               "rt:\t\t0x%X\n"
               "rd:\t\t0x%X\n"
               "shamt:\t0x%X\n"
               "funct:\t0x%X\n"
               "immed:\t0x%X\n"
               "addr:\t\t0x%X\n",
               myInstr.opcode,
               myInstr.rs,
               myInstr.rt,
               myInstr.rd,
               myInstr.shamt,
               myInstr.funct,
               myInstr.immed,
               myInstr.addr);
    }
    
    return myInstr;
}

//  LoadPR
void loadPR(){
    ifid = ifid_buff;
    idex = idex_buff;
    exmem = exmem_buff;
    memwb = memwb_buff;
}

void IF(){
    // Must deal with Stall, Flush, from Hazard Control Unit (see logic file)
    bool PCSrc;
    
    //Nominal Operation
    PCSrc = exmem.branch && exmem.ALUCompare;
    
    if (PCSrc){
        PC = exmem.branchTarget;
    }
    else{
        PC = ifid.pcplus4;
    }
    
    // Get the instruction for PC address
    ifid_buff.instr = decode(memory.fetchInstr(PC));
    
    // PCPLUS4 to buffer
    ifid_buff.pcplus4 = PC + 4;
    
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
    else if (!ifid.instr.type.compare(LW)){
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
    else if (!ifid.instr.type.compare(SW)){
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
    else if (!ifid.instr.type.compare(BEQ)){
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
    else{
        perror("Unable to derive constrol lines from instruction.");
    }

    idex_buff.regFileReadData1 = regFile.readReg(ifid.instr.rs);
    idex_buff.regFileReadData2 = regFile.readReg(ifid.instr.rt);
    idex_buff.pcplus4 = ifid.pcplus4;
    
    if (ifid.instr.immed >= 0x8000){
        idex_buff.signExtend = ifid.instr.immed + 0xFFFF0000;
    }
    else{
        idex_buff.signExtend = ifid.instr.immed;
    }
}

void EX(){
    unsigned int ALUInput1;
    unsigned int ALUInput2;
    unsigned int ALUControl;
    
    exmem_buff.regWrite = idex.regWrite;
    exmem_buff.memToReg = idex.memToReg;
    exmem_buff.branch = idex.branch;
    exmem_buff.memRead = idex.memRead;
    exmem_buff.memWrite = idex.memWrite;
        
    ALUInput1 = idex.regFileReadData1;
    
    if (idex.ALUSrc){
        ALUInput2 = idex.signExtend;
    }
    else{
        ALUInput2 = idex.regFileReadData2;
    }
    
    exmem_buff.ALUCompare = (ALUInput1 == ALUInput2);
    
    if (idex.ALUOp1 && !idex.ALUOp0){
        switch(idex.instr.funct){
            case 0x20:
                ALUControl = 0x2;
                break;
            case 0x22:
                ALUControl = 0x6;
                break;
            case 0x24:
                ALUControl = 0x0;
                break;
            case 0x25:
                ALUControl = 0x1;
                break;
            case 0x2A:
                ALUControl = 0x7;
                break;
            default:
                break;
        }
        
    }
    else if (!idex.ALUOp1 && !idex.ALUOp0){
        ALUControl = 0x2;
    }
    else if (!idex.ALUOp1 && idex.ALUOp0){
        ALUControl = 0x6;
    }
    
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
            exmem_buff.ALUResult = ALUInput1 + ALUInput2;
            break;
        case 0x6:
            // SUB
            // TODO: Does the order of input matter for the ALU Sub?
            exmem_buff.ALUResult = ALUInput1 - ALUInput2;
        case 0x7:
            // SLT
            if (ALUInput1 < ALUInput2){
                exmem_buff.ALUResult = 0x1;
            }
            else{
                exmem_buff.ALUResult = 0x0;
            }
        default:
            perror("Unknown ALU Controls");
            break;
    }
    
    exmem_buff.memWriteData = idex.regFileReadData2;
    
    if (idex.regDst){
        exmem_buff.regFileWriteReg = idex.instr.rd;
    }
    else{
        exmem_buff.regFileWriteReg = idex.instr.rt;
    }
    
}

void MEM(){
    memwb_buff.regWrite = exmem.regWrite;
    memwb_buff.memToReg = exmem.memToReg;
    
    if (exmem.memRead && !exmem.memWrite){
        memwb_buff.memReadData = memory.loadData(exmem.ALUResult);
    }
    else if (!exmem.memRead && exmem.memWrite){
        memwb_buff.memReadData = 0x0;
        memory.storeData(exmem.memWriteData, exmem.ALUResult);
    }
    else if (exmem.memRead && exmem.memWrite){
        perror("memRead and memWrite control lines both high.");
    }
    memwb_buff.memBypassData = exmem.ALUResult;
    memwb_buff.regFileWriteReg = exmem.regFileWriteReg;
}

void executeClockCycle(){
    IF();
    WB();
    ID();
    EX();
    MEM();
    loadPR();
}

void startup(){
    char file [] = MEMORYFILENAME;
    memory.importFile(file, PC);
    ifid.pcplus4 = PC;
}
int main(int argc, const char * argv[]) {
    startup();
    executeClockCycle();
    executeClockCycle();
    executeClockCycle();
    executeClockCycle();
    executeClockCycle();
    return 0;
}

