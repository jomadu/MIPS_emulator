//
//  main.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 2/16/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include <iostream>
#include <map>
bool debug = true;

int memory[1250]; // 1250 Words = 5kB of Memory

// Pipeline Registers
// IF_ID_PR - Instruction Fetch/Instruction Decode and Register Read Pipeline Register
std::map <std::string, unsigned int> IF_ID_PR;
// ID_EX_PR - Instruction Decode and Register Read/Execute Pipeline Register
std::map <std::string, unsigned int> ID_EX_PR;
// EX_MEM_PR - Execute/Memory Pipeline Register
std::map <std::string, unsigned int> EX_MEM_PR;
// MEM_WB_PR - Memory/WB Pipeline Register
std::map <std::string, unsigned int> MEM_WB_PR;

// Pipeline Register Buffers
// When LoadPR is called, the values in these structures are
// loaded into the above Pipeline Registers
//
// IF_ID_BUFFER - Instruction Fetch/Instruction Decode and Register Read Pipeline Register
std::map <std::string, unsigned int> IF_ID_BUFFER;
// ID_EX_BUFFER - Instruction Decode and Register Read/Execute Pipeline Register
std::map <std::string, unsigned int> ID_EX_BUFFER;
// EX_MEM_BUFFER - Execute/Memory Pipeline Register
std::map <std::string, unsigned int> EX_MEM_BUFFER;
// MEM_WB_BUFFER - Memory/WB Pipeline Register
std::map <std::string, unsigned int> MEM_WB_BUFFER;

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

//  Instruction Fetch
//  Instruction Decode and Register Read
//  Execute
//  Memory
//  WriteBack

//  LoadPR
void LoadPR(){
    IF_ID_PR = IF_ID_BUFFER;
    ID_EX_PR = IF_ID_BUFFER;
    EX_MEM_PR = EX_MEM_BUFFER;
    MEM_WB_PR = MEM_WB_BUFFER;
}



Instr decode(int mc){
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

void init_PR(){
    // Initialize the necesary control lines in each of the PR maps
    // TODO: Include the key strings as global variables
    IF_ID_PR["WB"] = 0;
    IF_ID_PR["MEM"] = 0;
    IF_ID_PR["EX"] = 0;
    IF_ID_PR["ID"] = 0;
    IF_ID_BUFFER["WB"] = 0;
    IF_ID_BUFFER["MEM"] = 0;
    IF_ID_BUFFER["EX"] = 0;
    IF_ID_BUFFER["ID"] = 0;
    
    ID_EX_PR["WB"] = 0;
    ID_EX_PR["MEM"] = 0;
    ID_EX_PR["EX"] = 0;
    ID_EX_BUFFER["WB"] = 0;
    ID_EX_BUFFER["MEM"] = 0;
    ID_EX_BUFFER["EX"] = 0;
    
    EX_MEM_PR["WB"] = 0;
    EX_MEM_PR["MEM"] = 0;
    EX_MEM_BUFFER["WB"] = 0;
    EX_MEM_BUFFER["MEM"] = 0;
    
    MEM_WB_PR["WB"] = 0;
    MEM_WB_BUFFER["WB"] = 0;
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
int main(int argc, const char * argv[]) {

    // int opCode = 0x014B4820; // Add t1, t2, t3
    int opCode = 0x21280004; // Addi t0, t1, 0x4
    Instr myInstr;
    myInstr = decode(opCode);
}

