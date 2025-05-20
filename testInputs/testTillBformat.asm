.text
_start:
    # R-format instructions
    add  t0, t1, t2
    sub  t3, t4, t5
    sll  t6, t0, t1

label_r1:
    slt  s0, s1, s2
    sltu s3, s4, s5
    xor  s6, s7, s8

label_r2:
    srl  s9, s10, s11
    sra  a0, a1, a2
    or   a3, a4, a5
    and  a6, a7, t3
    mul  t4, t5, t6

label_r3:
    div  t1, t2, t3
    rem  t0, t1, t2

    # I-format instructions
label_i1:
    addi t0, t1, 10
    slti t2, t3, 5
    sltiu t4, t5, 6

label_i2:
    xori t6, t0, 15
    ori  s0, s1, 4
    andi s2, s3, 12
    slli s4, s5, 2

label_i3:
    srli s6, s7, 1
    srai s8, s9, 3
    ld   s10, 0(s11)
    lb   a0, 4(s0)
f
label_i4:
    lh   a1, 8(s1)
    lw   a2, 12(s2)
    lbu  a3, 16(s3)
    lhu  a4, 20(s4)

    # S-format instructions
label_s1:
    sb   a5, 0(s5)
    sh   a6, 2(s6)

label_s2:
    sw   a7, 4(s7)
    sd   t0, 8(s8)

    # B-format instructions
label_b1:
    beq  t0, t1, label_r1
    bne  t2, t3, label_r2

label_b2:
    blt  t4, t5, label_i1
    bge  t6, s0, label_i2
    bltu s1, s2, label_s1
    bgeu s3, s4, label_s2
