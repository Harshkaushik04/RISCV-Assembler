.text
_start:
    sb t1, 0(t0)         # store byte at base
    sh t2, 4(t0)         # store half-word at offset 4
    sw t3, 8(t0)         # store word at offset 8
    sd t4, 16(t0)        # store doubleword at offset 16
