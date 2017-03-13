//
//  IFID_PR.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/10/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef IFID_PR_hpp
#define IFID_PR_hpp

#include <stdio.h>
#include "Instruction.hpp"

class IFID_PR{
public:
    unsigned int pcplus4;
    Instruction instr;
public:
    IFID_PR();
    void print();
};

#endif /* IFID_PR_hpp */
