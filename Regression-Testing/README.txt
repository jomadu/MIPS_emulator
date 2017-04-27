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
beq - tested, works
bne - tested, works
bgtz - tested, works
bltz - tested. works
blez - tested, works

***************Missing from this list***************
j - test written
jal - test written (combined with jr)
jr - test written (combined with jal)
movn - test written
movz - test written
xor - test written
xori - test written
lui - test written
seb - test written