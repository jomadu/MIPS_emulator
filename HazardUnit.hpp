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
#include "EXMEM_PR.hpp"

class HazardUnit{
public:
    bool stall;
public:
    HazardUnit();
    void update(IFID_PR ifid_buff, IDEX_PR idex_buff, IDEX_PR idex);
};

#endif /* HazardUnit_hpp */
