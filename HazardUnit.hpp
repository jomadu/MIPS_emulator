//
//  HazardUnit.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef HazardUnit_hpp
#define HazardUnit_hpp

#include <stdio.h>
#include "IDEX_PR.hpp"
#include "IFID_PR.hpp"

#define BEQ "BEQ"

class HazardUnit{
public:
    bool stall;
public:
    HazardUnit();
    void update(IFID_PR ifid_pr, IDEX_PR idex_pr);
};

#endif /* HazardUnit_hpp */
