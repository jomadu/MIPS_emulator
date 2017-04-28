Things to test:
addi - tested, works
addiu - tested, works
add - tested, works
and - tested, works
nor - DOES NOT WORK (just or instead of nor)
or  - tested, works
slt - tested, works
sltu - tested, works
sll - tested, works
srl - tested, works
sub - tested, works
subu - tested, works
div - we don't need to do this
divu - or this
mult - or this
multu - or this
andi - MOSTLY WORKS (this is supposed to zero extended, not sign extended, so upper 16 bytes are wrong sometimes)
addiu - is this supposed to be addu? if so, addu doesn't work
ori - MOSTLY WORKS (also should be zero extended)
slti - tested, works
sltiu - tested, works
lw - tested, works
lbu - tested, works
lb - tested, works
lhu - tested, works
lh - tested, works
sw - tested, works 
sh - tested, works
sb - tested, works
beq - tested, works (Max - I think we're stalling the right number of cycles, worth verifying though. This applies to all branches.)
bne - tested, works
bgtz - tested, works
bltz - tested, works
blez - tested, works
j - tested, works
jal - tested, works
jr - tested, works (combined with jal_test.txt)
movn - test written, works! :D
movz - test written, works! :D
xor - test written, works! :D
xori - test written, works! :D
lui - test written, works! :D
seb - test written, works! :D
