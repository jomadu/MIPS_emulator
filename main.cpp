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

using namespace std;

bool debug = true;

// Structs
struct Instr{
    unsigned int opcode = 0;
    unsigned int rs = 0;
    unsigned int rt = 0;
    unsigned int rd = 0;
    int shamt = 0;
    unsigned int funct = 0;
    int immed = 0;
    unsigned int addr = 0;
};

struct PipelineRegister{
    map <string, unsigned int> ctrl;
	map <string, unsigned int> logic;
    Instr instr;
};

// Memory
unsigned int MEMORY_SIZE = 1250;
unsigned int MEMORY[MEMORY_SIZE]; // 1250 Words = 5kB of Memory
unsigned int MEMORY_START = 0x0;

// Program Counter
unsigned int PC = 0x0;

// Pipeline Registers
PipelineRegister IFID_PR;
PipelineRegister IDEX_PR;
PipelineRegister EXMEM_PR;
PipelineRegister MEMWB_PR;

PipelineRegister IFID_BUFF;
PipelineRegister IDEX_BUFF;
PipelineRegister EXMEM_BUFF;
PipelineRegister MEMWB_BUFF;

// Map Key Strings
#define WBCtrl "WBCtrl"
#define MEMCtrl "MEMCtrl"
#define EXCtrl "EXCtrl"
#define INSTR "INSTR"
#define REGWRITE "REGWRITE"
#define STALL "STALL"
#define FLUSH "FLUSH"
#define PCPLUS4 "PCPLUS4"
#define RFWRITEREG "RFWRITEREG"
#define RFREADDATA1 "RFREADDATA1"
#define RFREADDATA2 "RFREADDATA2"
#define SIGNEX "SIGNEX"
#define BRANCHTARGET "BRANCHTARGET"
#define ALUCOMPARE "ALUCOMPARE"
#define ALURESULT "ALURESULT"
#define MEMWRITEDATA "MEMWRITEDATA"
#define REGDST "REGDST"
#define MEMREADDATA "MEMREADDATA"
#define MEMBYPASS "MEMBYPASS"
#define REGDST "REGDST"
#define ALUSRC "ALUSRC"
#define MEMTOREG "MEMTOREG"
#define REGWRITE "REGWRITE"
#define MEMREAD "MEMREAD"
#define MEMWRITE "MEMWRITE"
#define BRANCH "BRANCH"
#define ALUOP1 "ALUOP1"
#define ALUOP0 "ALUOP0"

void initPR(){
    // Initialize the necessary control lines in each of the PR maps
	//TODO: Initialize the logic lines for each
    
    IFID_PR.logic[STALL] = 0x0;
    IFID_PR.logic[FLUSH] = 0x0;
    IFID_PR.logic[PCPLUS4] = 0x0;
    
    IDEX_PR.ctrl[MEMTOREG] = 0x0;
    IDEX_PR.ctrl[REGWRITE] = 0x0;
    IDEX_PR.ctrl[MEMREAD] = 0x0;
    IDEX_PR.ctrl[MEMWRITE] = 0x0;
    IDEX_PR.ctrl[BRANCH] = 0x0;
    IDEX_PR.ctrl[EXCtrl] = 0x0;
    IDEX_PR.ctrl[ALUSRC] = 0x0;
    IDEX_PR.ctrl[ALUOP0] = 0x0;
    IDEX_PR.ctrl[ALUOP1] = 0x0;
    IDEX_PR.ctrl[REGDST] = 0x0;
    IDEX_PR.logic[PCPLUS4] = 0x0;
    IDEX_PR.logic[RFREADDATA1] = 0x0;
    IDEX_PR.logic[RFREADDATA2] = 0x0;
    IDEX_PR.logic[SIGNEX] = 0x0;
    
    EXMEM_PR.ctrl[MEMTOREG] = 0x0;
    EXMEM_PR.ctrl[REGWRITE] = 0x0;
    EXMEM_PR.ctrl[MEMREAD] = 0x0;
    EXMEM_PR.ctrl[MEMWRITE] = 0x0;
    EXMEM_PR.ctrl[BRANCH] = 0x0;
    EXMEM_PR.logic[BRANCHTARGET] = 0x0;
    EXMEM_PR.logic[ALUCOMPARE] = 0x0;
    EXMEM_PR.logic[ALURESULT] = 0x0;
    EXMEM_PR.logic[MEMWRITEDATA] = 0x0;
    EXMEM_PR.logic[REGDST] = 0x0;
    
    MEMWB_PR.ctrl[MEMTOREG] = 0x0;
    MEMWB_PR.ctrl[REGWRITE] = 0x0;
    MEMWB_PR.logic[RFWRITEREG] = 0x0;
    MEMWB_PR.logic[MEMREADDATA] = 0x0;
    
    IFID_BUFF.logic[STALL] = 0x0;
    IFID_BUFF.logic[FLUSH] = 0x0;
    IFID_BUFF.logic[PCPLUS4] = 0x0;
    
    IDEX_BUFF.ctrl[MEMTOREG] = 0x0;
    IDEX_BUFF.ctrl[REGWRITE] = 0x0;
    IDEX_BUFF.ctrl[MEMREAD] = 0x0;
    IDEX_BUFF.ctrl[MEMWRITE] = 0x0;
    IDEX_BUFF.ctrl[BRANCH] = 0x0;
    IDEX_BUFF.ctrl[EXCtrl] = 0x0;
    IDEX_BUFF.ctrl[ALUSRC] = 0x0;
    IDEX_BUFF.ctrl[ALUOP0] = 0x0;
    IDEX_BUFF.ctrl[ALUOP1] = 0x0;
    IDEX_BUFF.ctrl[REGDST] = 0x0;
    IDEX_BUFF.logic[PCPLUS4] = 0x0;
    IDEX_BUFF.logic[RFREADDATA1] = 0x0;
    IDEX_BUFF.logic[RFREADDATA2] = 0x0;
    IDEX_BUFF.logic[SIGNEX] = 0x0;
    
    EXMEM_BUFF.ctrl[MEMTOREG] = 0x0;
    EXMEM_BUFF.ctrl[REGWRITE] = 0x0;
    EXMEM_BUFF.ctrl[MEMREAD] = 0x0;
    EXMEM_BUFF.ctrl[MEMWRITE] = 0x0;
    EXMEM_BUFF.ctrl[BRANCH] = 0x0;
    EXMEM_BUFF.logic[BRANCHTARGET] = 0x0;
    EXMEM_BUFF.logic[ALUCOMPARE] = 0x0;
    EXMEM_BUFF.logic[ALURESULT] = 0x0;
    EXMEM_BUFF.logic[MEMWRITEDATA] = 0x0;
    EXMEM_BUFF.logic[REGDST] = 0x0;
    
    MEMWB_BUFF.ctrl[MEMTOREG] = 0x0;
    MEMWB_BUFF.ctrl[REGWRITE] = 0x0;
    MEMWB_BUFF.logic[RFWRITEREG] = 0x0;
    MEMWB_BUFF.logic[MEMREADDATA] = 0x0;
    
    return;
}

//  LoadPR
void loadPR(){
    IFID_PR = IFID_BUFF;
    IDEX_PR = IFID_BUFF;
    EXMEM_PR = EXMEM_BUFF;
    MEMWB_PR = MEMWB_BUFF;
    
    return;
}


unsigned int memoryIdx(unsigned int memoryAddr){
    return (memoryAddr - MEMORY_START)/4;
}

// Fetch Instruction from low MEMORY
unsigned int fetchInstr(unsigned int addr){
    int memIdx = memoryIdx(addr);
    return MEMORY[memIdx];
}
// Store Instruction in low MEMORY
void storeInstr(unsigned int mc, unsigned int addr){
    int memIdx = memoryIdx(addr);
    MEMORY[memIdx] = mc;
    return;
}

// Load Data from high MEMORY
unsigned int loadData(unsigned int addr){
    // FUTURE TODO: This may screw things up royally (off by one error?)
    int memIdx = MEMORY_SIZE - memoryIdx(addr);
    return MEMORY[memIdx];
}

// Store Data in high MEMORY
void storeData(unsigned int data, unsigned int addr){
    // FUTURE TODO: SEE ABOVE OR BEAR THE CONSEQUENCES
    int memIdx = MEMORY_SIZE - memoryIdx(addr);
    MEMORY[memIdx] = data;
    return;
}


Instr decode(unsigned int addr){
    //  MCDecode(MC)
    //  Input -> MC in 8 digit HEX int
    //  Returns -> Instruction Object
    Instr myInstr;
    
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
    
    unsigned int mc = loadData(addr);
    
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
    
    
}

void WB(){
}

void ID(){
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

void importMemory(char * file){
    char buffer[80];
    char cAddr[80];
    char cData[80];
    string sAddr;
    string sData;
    int iAddr;
    int iData;
    
    FILE *fp;
    int i = 0;
    stringstream ss;
    
    
    if ((fp = fopen(file,"r")) == NULL)
    {
        printf("Could not open %s\n",file);
        exit(1);
    }
    
    while ( !feof(fp))
    {
        // read in the line and make sure it was successful
        if (fgets(buffer,500,fp) != NULL)
        {
            sscanf(buffer, "%s  %s", cAddr, cData);
            
            ss << cAddr;
            ss >> sAddr;
            iAddr = stoi(sAddr.substr(2,8),nullptr,16);
            
            ss.clear();
            
            ss << cData;
            ss >> sData;
            iData = stoi(sData.substr(2,8),nullptr,16);
            
            ss.clear();
            
            if(i == 0){
                MEMORY_START = iAddr;
            }
            
            storeData(iData, iAddr);
            
            printf("%d: %s",i++,buffer);
        }
    }
}

int main(int argc, const char * argv[]) {
    char file [] = "memory.txt";
    initPR();
    importMemory(file);
    return 0;
}

