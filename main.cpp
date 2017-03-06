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

int MEMORY[1250]; // 1250 Words = 5kB of Memory
int MEMORY_START;

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
string WBCtrl = "WBCtrl";
string MEMCtrl = "MEMCtrl";
string EXCtrl = "EXCtrl";
string INSTR = "INSTRCtrl";
string REGWRITE = "REGWRITE";

void init_PR(){
    // Initialize the necessary control lines in each of the PR maps
	//TODO: Initialize the logic lines for each
    
    IDEX_PR.ctrl[WBCtrl] = 0x0;
    IDEX_PR.ctrl[MEMCtrl] = 0x0;
    IDEX_PR.ctrl[EXCtrl] = 0x0;
    IDEX_BUFF.ctrl[WBCtrl] = 0x0;
    IDEX_BUFF.ctrl[MEMCtrl] = 0x0;
    IDEX_BUFF.ctrl[EXCtrl] = 0x0;
    
    EXMEM_PR.ctrl[WBCtrl] = 0x0;
    EXMEM_PR.ctrl[MEMCtrl] = 0x0;
    EXMEM_BUFF.ctrl[WBCtrl] = 0x0;
    EXMEM_BUFF.ctrl[MEMCtrl] = 0x0;
    
    MEMWB_PR.ctrl[WBCtrl] = 0x0;
    MEMWB_BUFF.ctrl[WBCtrl] = 0x0;
    
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

Instr decode(int mc){
    //  MCDecode(MC)
    //  Input -> MC in 8 digit HEX int
    //  Returns -> Instruction Object
	// TODO: make input address instead of machine code
    Instr myInstr;
    
    unsigned int opcodeMask = 0xFC000000;
    unsigned int rsMask = 0x3E00000;
    unsigned int rtMask = 0x1F0000;
    unsigned int rdMask = 0xF800;
    unsigned int shamtMask = 0x7C0;
    unsigned int functMask = 0x1F;
    unsigned int immedMask = 0xFFFF;
    unsigned int addrMask = 0x3FFFFFF;

	// TODO: Instantiate the shift amounts for masked elements
    
    myInstr.opcode = (mc & opcodeMask) >> 26;
    myInstr.rs = (mc & rsMask) >> 21;
    myInstr.rt = (mc & rtMask) >> 16;
    myInstr.rd = (mc & rdMask) >> 11;
    myInstr.shamt = (mc & shamtMask) >> 6;
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
}

void WB(){
}

void ID(){
}

void EX(){
}

void MEM(){
}

void execute_clock_cycle(){
    IF();
    WB();
    ID();
    EX();
    MEM();
}

unsigned int memoryIdx(unsigned int memoryAddr){
    return (memoryAddr - MEMORY_START)/4;
}

void import_memory_V2(char * file){
    char buffer[80];
    char c_addr[80];
    char c_val[80];
    string s_addr;
    string s_val;
    int hex_addr;
    int hex_val;
    
    FILE *fp;
    int i = 0;
    int memIdx = 0;
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
            sscanf(buffer, "%s  %s", c_addr, c_val);
            
            ss << c_addr;
            ss >> s_addr;
            hex_addr = stoi(s_addr.substr(2,8),nullptr,16);
            
            ss.clear();
            
            ss << c_val;
            ss >> s_val;
            hex_val = stoi(s_val.substr(2,8),nullptr,16);
            
            ss.clear();
            
            if(i == 0){
                MEMORY_START = hex_addr;
            }
            
            memIdx = memoryIdx(hex_addr);
            MEMORY[memIdx] = hex_val;
            
            printf("%d: %s",i++,buffer);
        }
    }
}

int main(int argc, const char * argv[]) {

    
    char file [] = "memory.txt";
    // int opCode = 0x014B4820; // Add t1, t2, t3
    int mc = 0x21280004; // Addi t0, t1, 0x4
    init_PR();
    import_memory_V2(file);
    return 0;
}

