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
#define J   "J"
#define I   "I"
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
    myInstr.shamt = (mc & shamtMask) >> shamtShift;
    myInstr.funct = (mc & functMask);
    myInstr.immed = (mc & immedMask);
    myInstr.addr = (mc & addrMask);
    
    // Note: These cases aren't bulletproof
    switch (myInstr.opcode) {
        case 0x0:
            myInstr.type = R;
            break;
        case 0x23:
            myInstr.type = LW;
            break;
        case 0x2B:
            myInstr.type = SW;
            break;
        case 0x4:
            myInstr.type = BEQ;
            break;
        case 0x2:
            myInstr.type = LW;
            break;
        default:
            myInstr.type = I;
            break;
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
    else if (!ifid.instr.type.compare(J)){
        //TODO: Implement Jump control lines
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
        perror("Unable to derive constrol lines from instruction.\n");
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
    unsigned int ALUInput1U;
    unsigned int ALUInput2U;
    int ALUInput1;
    int ALUInput2;
    unsigned int ALUControl;
    bool unsignedFlag = false;
    unsigned int shamt;
    unsigned int shamtMask = 0x3E0;
    
    exmem_buff.regWrite = idex.regWrite;
    exmem_buff.memToReg = idex.memToReg;
    exmem_buff.branch = idex.branch;
    exmem_buff.memRead = idex.memRead;
    exmem_buff.memWrite = idex.memWrite;
    
    exmem_buff.branchTarget = idex.pcplus4 + (idex.signExtend + 4);
    
    ALUInput1U = idex.regFileReadData1;
    ALUInput1 = (int) ALUInput1U;
    
    if (idex.ALUSrc){
        ALUInput2U = idex.signExtend;
        ALUInput2 = (int) ALUInput2U;

    }
    else{
        ALUInput2U = idex.regFileReadData2;
        ALUInput2 = (int) ALUInput2U;
    }
    
    exmem_buff.ALUCompare = (ALUInput1 == ALUInput2);
    
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
        perror("memRead and memWrite control lines both high.\n");
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
    if (debug){
        ifid.print();
        idex.print();
        exmem.print();
        memwb.print();
        regFile.print();
    }
}

void startup(){
    char file [] = MEMORYFILENAME;
    memory.importFile(file, PC);
    ifid.pcplus4 = PC;
}
int main(int argc, const char * argv[]) {
    startup();
                         // Instruction 1&4     | Instruction 2&5   | Instruction 3&6
    executeClockCycle(); // IF()                |                   |
    executeClockCycle(); // ID()                |                   |
    executeClockCycle(); // EX()                |                   |
    executeClockCycle(); // MEM()               | IF()              |
    executeClockCycle(); // WB()                | ID()              |
    executeClockCycle(); //                     | EX()              |
    executeClockCycle(); //                     | MEM()             | IF()
    executeClockCycle(); //                     | WB()              | ID()
    executeClockCycle(); //                     |                   | EX()
    executeClockCycle(); // IF()                |                   | MEM()
    executeClockCycle(); // ID()                |                   | WB()
    executeClockCycle(); // EX()                |                   |
    executeClockCycle(); // MEM()               | IF()              |
    executeClockCycle(); // WB()                | ID()              |
    executeClockCycle(); //                     | EX()              |
    executeClockCycle(); //                     | MEM()             | IF()
    executeClockCycle(); //                     | WB()              | ID()
    executeClockCycle(); //                     |                   | EX()
    executeClockCycle(); //                     |                   | MEM()
    executeClockCycle(); //                     |                   | WB()

    return 0;
}

