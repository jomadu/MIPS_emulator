//
//  ForwardingUnit.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef ForwardingUnit_hpp
#define ForwardingUnit_hpp

#include <stdio.h>
#include "IFID_PR.hpp"
#include "IDEX_PR.hpp"
#include "EXMEM_PR.hpp"
#include "MEMWB_PR.hpp"

#define BEQ "BEQ"
#define BNE "BNE"
#define BGTZ "BGTZ"
#define BLTZ "BLTZ"
#define BLEZ "BLEZ"
#define J   "J"
#define JAL  "JAL"
#define JR   "JR"

class ForwardingUnit{
public:
    unsigned int forwardA;
    unsigned int forwardB;
    unsigned int branchForwardA;
    unsigned int branchForwardB;
    unsigned int jumpForwardA;
    unsigned int jumpForwardB;
public:
    ForwardingUnit();
    void updateSTD(IDEX_PR idex, EXMEM_PR exmem, MEMWB_PR memwb);
    void updateBranching(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb);
    void updateJumping(IFID_PR ifid, EXMEM_PR exmem, MEMWB_PR memwb);
};

#endif /* ForwardingUnit_hpp */
