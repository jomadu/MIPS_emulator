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
#include "IDEX_PR.hpp"
#include "EXMEM_PR.hpp"
#include "MEMWB_PR.hpp"

class ForwardingUnit{
public:
    unsigned int forwardA;
    unsigned int forwardB;
public:
    ForwardingUnit();
    void update(IDEX_PR idex_pr, EXMEM_PR exmem_pr, MEMWB_PR memwb_pr);
};

#endif /* ForwardingUnit_hpp */
