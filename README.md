# SISYPHUS 
## The MIPS EMULATOR that won't QUIT!
### Author: Max Dunn & Thierry Shimoda

## System Requirements
- Linux or Mac OSX environment
- g++ compiler

## How To Run:

1. Put all unzipped files into a single directory.
   - Note: The exception to this is the ‘../Program/‘ directory,
   which contains Program1File1.txt and Program2File1.txt.
   - Note: If you replace either Program with a new file,
     make sure to adjust `PROGRAM1` or `PROGRAM2` `#defines` in
     Testbench.hpp. (see above)
2. Configure the Testbench.hpp to suit different 
   cache variations. (see above)
3. In terminal, navigate to the containing directory
4. Run -> 'make'. This will compile the source code.
5. Run -> './sisyphus_mips_emulator'. This will run the 
   program simulation. Output will be to Terminal.
6. On termination of the program, the code outputs:
   1) The entire memory of our program
   2) A summarized output
    
   Example Output:
   ```
   *************************************************************
   *  _______                          __   __                 *
   * |    ___|.--.--.-----.----.--.--.|  |_|__|.-----.-----.   *
   * |    ___||_   _|  -__|  __|  |  ||   _|  ||  _  |     |   *
   * |_______||__.__|_____|____|_____||____|__||_____|__|__|   *
   *  ______                        __         __          __  *
   * |      |.-----.--------.-----.|  |.-----.|  |_.-----.|  | *
   * |   ---||  _  |        |  _  ||  ||  -__||   _|  -__||__| *
   * |______||_____|__|__|__|   __||__||_____||____|_____||__| *
   *                        |__|                               *
   *************************************************************
   *                                                           *
   *   Inputs:                                                 *
   *     \                                                     *
   *      \--> iCache Size = 256                               *
   *     \                                                     *
   *      \--> dCache Size = 128                               *
   *     \                                                     *
   *      \--> Block Fill Size = 1                             *
   *     \                                                     *
   *      \--> Cache Write Policy = Write Back                 *
   *     \                                                     *
   *      \--> Program = Programs/Program2File1.txt            *
   *                                                           *
   *   Results:                                                *
   *     \                                                     *
   *      \--> Memory[6] = 1                                   *
   *     \                                                     *
   *      \--> Memory[7] = 543780465                           *
   *     \                                                     *
   *      \--> Memory[8] = 544109430                           *
   *     \                                                     *
   *      \--> Memory[9] = 543780465                           *
   *     \                                                     *
   *      \--> iCache Hit Rate = 99.10886%                     *
   *     \                                                     *
   *      \--> dCache Hit Rate = 68.16000%                     *
   *     \                                                     *
   *      \--> # of instructions = 15598                       *
   *     \                                                     *
   *      \--> Clock Cycles = 22986                            *
   *     \                                                     *
   *      \--> CPI = 1.474                                     *
   *                                                           *
   *************************************************************
   ```
## How To Configure

1. Open Testbench.hpp in a text editor.
2. Change relevant parameters. Save.
