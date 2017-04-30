//
//  Constants.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/28/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Constants_hpp
#define Constants_hpp

#define R   "R"
#define LW  "LW"
#define LB "LB"
#define LH "LH"
#define LHU  "LHU"
#define LBU  "LBU"
#define SW  "SW"
#define SH "SH"
#define SB  "SB"
#define BEQ "BEQ"
#define BNE "BNE"
#define BGTZ "BGTZ"
#define BLTZ "BLTZ"
#define BLEZ "BLEZ"
#define J   "J"
#define JAL  "JAL"
#define JR "JR"
#define I   "I"

#define BYTE0_MASK 0x000000FF
#define BYTE1_MASK 0x0000FF00
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0xFF000000

#define HWL_MASK 0x0000FFFF
#define HWM_MASK 0x00FFFF00
#define HWH_MASK 0xFFFF0000

#define WORD_SIZE 32 // Please do not change this. It will break everything if you do.

#endif /* Constants_hpp */
