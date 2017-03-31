//
//  IFID_PR.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "IFID_PR.hpp"

IFID_PR::IFID_PR(){
    pcnext = 0x0;
};

void IFID_PR::print(){
    printf("------------------------\n"
           "IFID Pipeline Register\n"
           "------------------------\n");
    instr.print();
    printf("\nOther\n"
           "------------\n");
    printf("PC + 4:\t\t\t\t0x%X\n\n", pcnext);
}
