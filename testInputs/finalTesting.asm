.data
array:      .word 1, 3, 5, 7, 9, 11, 13, 15
array_len:  .word 8
target:     .word 11

.text

_start:
LUI a0, 0x10000
LUI t0, 0x10000
LW a1, 32(t0)
LUI t0, 0x10000
LW a2, 36(t0)
JAL ra, binary_search

binary_search:
ADDI t0, x0, 0
ADDI t1, a1, -1

loop:
BGTU t0, t1, not_found
SUB t2, t1, t0 
SRLI t2, t2, 1
ADD t2, t2, t0
SLLI t3, t2, 2
ADD t3, a0, t3
LW t4, 0(t3)
BEQ t4, a2, found
BLT t4, a2, go_right
ADDI t1, t2, -1
JAL x0, loop

go_right:
ADDI t0, t2, 1
JAL x0, loop

found:
ADD a0, x0, t2
JALR x0, 0(ra)

not_found:
ADDI a0, x0, -1
JALR x0, 0(ra)
