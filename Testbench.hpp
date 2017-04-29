//
//  Testbench.hpp
//  MIPS-emulator
//
//  Created by Max Dunn on 4/28/17.
//  Copyright © 2017 Max Dunn. All rights reserved.
//

#ifndef Testbench_h
#define Testbench_h

#define MEMORY_SIZE 5000

#define I_CACHE_SIZE 64
#define D_CACHE_SIZE 64

#define CACHE_SET_FILL 4 // equivalent to block fill size

#define CACHE_WRITE_POLICY 1 //0 - write back, 1 - write through

#define CACHE_W_STARTUP_P 3
#define CACHE_W_SUBSQT_P 1

#define CACHE_R_STARTUP_P 4
#define CACHE_R_SUBSQT_P 1

#define DEBUG true


#endif /* Testbench_h */
