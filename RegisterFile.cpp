//
//  RegisterFile.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "RegisterFile.hpp"
#include "Constants.hpp"
#include "Testbench.hpp"

RegisterFile::RegisterFile(){
    zero = 0x0;
    at = 0x0;
    v0 = 0x0;
    v1 = 0x0;
    a0 = 0x0;
    a1 = 0x0;
    a2 = 0x0;
    a3 = 0x0;
    t0 = 0x0;
    t1 = 0x0;
    t2 = 0x0;
    t3 = 0x0;
    t4 = 0x0;
    t5 = 0x0;
    t6 = 0x0;
    t7 = 0x0;
    t8 = 0x0;
    t9 = 0x0;
    s0 = 0x0;
    s1 = 0x0;
    s2 = 0x0;
    s3 = 0x0;
    s4 = 0x0;
    s5 = 0x0;
    s6 = 0x0;
    s7 = 0x0;
    k0 = 0x0;
    k1 = 0x0;
    gp = 0x0;
    sp = 0x0;
    fp = 0x0;
    ra = 0x0;
}
void RegisterFile::writeReg(unsigned int regNum, unsigned int data){
    switch (regNum) {
        case 0:
            //Cannot write to register zero
            break;
        case 1:
            at = data;
            break;
        case 2:
            v0 = data;
            break;
        case 3:
            v1 = data;
            break;
        case 4:
            a0 = data;
            break;
        case 5:
            a1 = data;
            break;
        case 6:
            a2 = data;
            break;
        case 7:
            a3 = data;
            break;
        case 8:
            t0 = data;
            break;
        case 9:
            t1 = data;
            break;
        case 10:
            t2 = data;
            break;
        case 11:
            t3 = data;
            break;
        case 12:
            t4 = data;
            break;
        case 13:
            t5 = data;
            break;
        case 14:
            t6 = data;
            break;
        case 15:
            t7 = data;
            break;
        case 16:
            s0 = data;
            break;
        case 17:
            s1 = data;
            break;
        case 18:
            s2 = data;
            break;
        case 19:
            s3 = data;
            break;
        case 20:
            s4 = data;
            break;
        case 21:
            s5 = data;
            break;
        case 22:
            s6 = data;
            break;
        case 23:
            s7 = data;
            break;
        case 24:
            t8 = data;
            break;
        case 25:
            t9 = data;
            break;
        case 26:
            k0 = data;
            break;
        case 27:
            k1 = data;
            break;
        case 28:
            gp = data;
            break;
        case 29:
            sp = data;
            break;
        case 30:
            fp = data;
            break;
        case 31:
            ra = data;
            break;
        default:
            perror("Could not write to RegisterFile (idx out of bounds)\n");
    }
}

int RegisterFile::readReg(unsigned int regNum){
    switch (regNum) {
        case 0:
            // Never write to the zero register
            return zero;
        case 1:
            return at;
        case 2:
            return v0;
        case 3:
            return v1;
        case 4:
            return a0;
        case 5:
            return a1;
        case 6:
            return a2;
        case 7:
            return a3;
        case 8:
            return t0;
        case 9:
            return t1;
        case 10:
            return t2;
        case 11:
            return t3;
        case 12:
            return t4;
        case 13:
            return t5;
        case 14:
            return t6;
        case 15:
            return t7;
        case 16:
            return s0;
        case 17:
            return s1;
        case 18:
            return s2;
        case 19:
            return s3;
        case 20:
            return s4;
        case 21:
            return s5;
        case 22:
            return s6;
        case 23:
            return s7;
        case 24:
            return t8;
        case 25:
            return t9;
        case 26:
            return k0;
        case 27:
            return k1;
        case 28:
            return gp;
        case 29:
            return sp;
        case 30:
            return fp;
        case 31:
            return ra;
        default:
            perror("Could not read from RegisterFile (idx out of bounds)\n");
    }
    return 0;
}

void RegisterFile::print(){
    printf("|--------------------------|\n"
           "| Register File            |\n"
           "|--------------------------|\n"
           "|zero (0x%-2X) | 0x%-8X |\n"
           "|at   (0x%-2X) | 0x%-8X |\n"
           "|v0   (0x%-2X) | 0x%-8X |\n"
           "|v1   (0x%-2X) | 0x%-8X |\n"
           "|a0   (0x%-2X) | 0x%-8X |\n"
           "|a1   (0x%-2X) | 0x%-8X |\n"
           "|a2   (0x%-2X) | 0x%-8X |\n"
           "|a3   (0x%-2X) | 0x%-8X |\n"
           "|t0   (0x%-2X) | 0x%-8X |\n"
           "|t1   (0x%-2X) | 0x%-8X |\n"
           "|t2   (0x%-2X) | 0x%-8X |\n"
           "|t3   (0x%-2X) | 0x%-8X |\n"
           "|t4   (0x%-2X) | 0x%-8X |\n"
           "|t5   (0x%-2X) | 0x%-8X |\n"
           "|t6   (0x%-2X) | 0x%-8X |\n"
           "|t7   (0x%-2X) | 0x%-8X |\n"
           "|t8   (0x%-2X) | 0x%-8X |\n"
           "|t9   (0x%-2X) | 0x%-8X |\n"
           "|s0   (0x%-2X) | 0x%-8X |\n"
           "|s1   (0x%-2X) | 0x%-8X |\n"
           "|s2   (0x%-2X) | 0x%-8X |\n"
           "|s3   (0x%-2X) | 0x%-8X |\n"
           "|s4   (0x%-2X) | 0x%-8X |\n"
           "|s5   (0x%-2X) | 0x%-8X |\n"
           "|s6   (0x%-2X) | 0x%-8X |\n"
           "|s7   (0x%-2X) | 0x%-8X |\n"
           "|k0   (0x%-2X) | 0x%-8X |\n"
           "|k1   (0x%-2X) | 0x%-8X |\n"
           "|gp   (0x%-2X) | 0x%-8X |\n"
           "|sp   (0x%-2X) | 0x%-8X |\n"
           "|fp   (0x%-2X) | 0x%-8X |\n"
           "|ra   (0x%-2X) | 0x%-8X |\n"
           "|--------------------------|\n\n",
           0,zero,
           1,at,
           2,v0,
           3,v1,
           4,a0,
           5,a1,
           6,a2,
           7,a3,
           8,t0,
           9,t1,
           10,t2,
           11,t3,
           12,t4,
           13,t5,
           14,t6,
           15,t7,
           16,t8,
           17,t9,
           18,s0,
           19,s1,
           20,s2,
           21,s3,
           22,s4,
           23,s5,
           24,s6,
           25,s7,
           26,k0,
           27,k1,
           28,gp,
           29,sp,
           30,fp,
           31,ra);
}

