.text
_start:
    add  x5, x0, x0      # x5 = 0
    add  x6, x5, x5      # x6 = 0
    sub  x7, x6, x5      # x7 = 0
    sll  x8, x6, x5      # x8 = 0
    slt  x9, x6, x5      # x9 = 0
    sltu x10, x6, x5     # x10 = 0
    xor  x11, x6, x5     # x11 = 0
    srl  x12, x6, x5     # x12 = 0
    sra  x13, x6, x5     # x13 = 0
    or   x14, x6, x5     # x14 = 0
    and  x15, x6, x5     # x15 = 0
    mul  x16, x6, x5     # x16 = 0 (if M-extension supported)
    div  x17, x6, x5     # x17 = 0 (division by 0: behavior depends on implementation)
    rem  x18, x6, x5     # x18 = 0 (remainder of 0 / 0: implementation-defined)
