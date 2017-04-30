//
//  RegisterFile.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef RegisterFile_hpp
#define RegisterFile_hpp

#include <stdio.h>
#include "Constants.hpp"
#include "Testbench.hpp"

class RegisterFile{
public:
    unsigned int zero;
    unsigned int at;
    unsigned int v0;
    unsigned int v1;
    unsigned int a0;
    unsigned int a1;
    unsigned int a2;
    unsigned int a3;
    unsigned int t0;
    unsigned int t1;
    unsigned int t2;
    unsigned int t3;
    unsigned int t4;
    unsigned int t5;
    unsigned int t6;
    unsigned int t7;
    unsigned int t8;
    unsigned int t9;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int k0;
    unsigned int k1;
    unsigned int gp;
    unsigned int sp;
    unsigned int fp;
    unsigned int ra;
public:
    RegisterFile();
    void writeReg(unsigned int regNum, unsigned int data);
    int readReg(unsigned int regNum);
    void print();
};

#endif /* RegisterFile_hpp */
