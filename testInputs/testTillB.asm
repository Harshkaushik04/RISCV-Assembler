.text
_start:
    beq x1, x2, label1
    bne x3, x4, label2
    blt x5, x6, label3
    bge x7, x8, label4
    bltu x9, x10, label5
    bgeu x11, x12, label6

label1:
    add x13, x0, x0

label2:
    sub x14, x0, x0

label3:
    or x15, x0, x0

label4:
    and x16, x0, x0
label5:
    xor x17, x0, x0

label6:
    sll x18, x0, x0
