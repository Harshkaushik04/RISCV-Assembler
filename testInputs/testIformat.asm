.text
_start:
    # Immediate arithmetic/logical
    addi x5, x0, 10       # x5 = 10
    slti x6, x5, 20       # x6 = (x5 < 20) ? 1 : 0
    sltiu x7, x5, 20      # x7 = (x5 < 20) unsigned
    xori x8, x5, 0xF0     # x8 = x5 ^ 0xF0
    ori  x9, x5, 0x0F     # x9 = x5 | 0x0F
    andi x10, x5, 0xFF    # x10 = x5 & 0xFF

    # Shift immediate
    slli x11, x5, 2       # x11 = x5 << 2
    srli x12, x5, 1       # x12 = x5 >> 1 (logical)
    srai x13, x5, 1       # x13 = x5 >> 1 (arithmetic)

    # Load instructions (assume address in x14 points to valid memory)
    addi x14, x0, 0x1000  # x14 = base address

    lb   x15, 0(x14)      # x15 = byte at [x14]
    lh   x16, 2(x14)      # x16 = halfword at [x14+2]
    lw   x17, 4(x14)      # x17 = word at [x14+4]
    ld   x18, 8(x14)      # x18 = doubleword at [x14+8]
    lbu  x19, 10(x14)     # x19 = unsigned byte
    lhu  x20, 12(x14)     # x20 = unsigned halfword
