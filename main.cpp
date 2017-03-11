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


//  LoadPR
void loadPR(){
    ifid = ifid_buff;
    idex = idex_buff;
    exmem = exmem_buff;
    memwb = memwb_buff;
}

Instruction decode(unsigned int addr){
    //  MCDecode(MC)
    //  Input -> MC in 8 digit HEX int
    //  Returns -> Instruction Object
    Instruction myInstr;
    
    unsigned int opcodeMask = 0xFC000000;
    unsigned int rsMask = 0x3E00000;
    unsigned int rtMask = 0x1F0000;
    unsigned int rdMask = 0xF800;
    unsigned int shamtMask = 0x7C0;
    unsigned int functMask = 0x1F;
    unsigned int immedMask = 0xFFFF;
    unsigned int addrMask = 0x3FFFFFF;
    
    unsigned int opcodeShift = 26;
    unsigned int rsShift = 21;
    unsigned int rtShift = 16;
    unsigned int rdShift = 11;
    unsigned int shamtShift = 6;
    
    unsigned int mc = memory.fetchInstr(addr);
    
    myInstr.opcode = (mc & opcodeMask) >> opcodeShift;
    myInstr.rs = (mc & rsMask) >> rsShift;
    myInstr.rt = (mc & rtMask) >> rtShift;
    myInstr.rd = (mc & rdMask) >> rdShift;
    myInstr.shamt = (mc & shamtMask) >> shamtMask;
    myInstr.funct = (mc & functMask);
    myInstr.immed = (mc & immedMask);
    myInstr.addr = (mc & addrMask);
    
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

}

void ID(){
    //Update Hazard Detection Unit
}

void EX(){
}

void MEM(){
}

void executeClockCycle(){
    IF();
    WB();
    ID();
    EX();
    MEM();
}

int main(int argc, const char * argv[]) {
    char file [] = "memory.txt";
    memory.importFile(file);
    return 0;
}

