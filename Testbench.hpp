//
//  Testbench.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/28/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Testbench_hpp
#define Testbench_hpp

#define MEMORY_SIZE 4800

#define I_CACHE_SIZE 64
#define D_CACHE_SIZE 1024

#define CACHE_SET_FILL 4 // equivalent to block fill size

#define CACHE_WRITE_POLICY 0 //0 - write back, 1 - write through

#define CACHE_W_STARTUP_P 1 // 6
#define CACHE_W_SUBSQT_P 1 // 2

#define CACHE_R_STARTUP_P 1 // 8
#define CACHE_R_SUBSQT_P 1 // 2

#define DEBUG true

#define PROGRAM 1

#define TESTPROGRAM "Regression-Testing/add_test.txt"
#define PROGRAM1 "Programs/Program1File1.txt"
#define PROGRAM2 "Programs/Program2File1.txt"


#endif /* Testbench_hpp */
