//
//  RegisterFile.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "RegisterFile.hpp"

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
void RegisterFile::setReg(int regNum, unsigned int data){
    switch (regNum) {
        case 0:
            // Never write to the zero register
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
            break;
    }
}
