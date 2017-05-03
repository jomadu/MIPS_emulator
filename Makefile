CC=g++
CFLAGS=-Wall -std=c++11
LDFLAGS=
SOURCES=main.cpp Instruction.cpp Cache.cpp Memory.cpp RegisterFile.cpp ForwardingUnit.cpp HazardUnit.cpp IFID_PR.cpp IDEX_PR.cpp EXMEM_PR.cpp MEMWB_PR.cpp  
EXECUTABLE=sisyphus_mips_emulator

target: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)
    