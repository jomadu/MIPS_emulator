//
//  HazardUnit.cpp
//  MIPS-emulator
//
//  Created by Max Dunn on 3/11/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#include "HazardUnit.hpp"
#include "IFID_PR.hpp"
#include "IDEX_PR.hpp"

HazardUnit::HazardUnit(){
    stall = false;
}

void HazardUnit::update(IFID_PR ifid_pr, IDEX_PR idex_pr){
    stall = (idex_pr.memRead) && ((idex_pr.instr.rt == ifid_pr.instr.rs) || (idex_pr.instr.rt == ifid_pr.instr.rt));
    
    if ((idex_pr.memRead || !ifid_pr.instr.type.compare(BEQ)) && ((idex_pr.instr.rt == ifid_pr.instr.rs) || (idex_pr.instr.rt == ifid_pr.instr.rt))){
        stall = true;
    }
}
