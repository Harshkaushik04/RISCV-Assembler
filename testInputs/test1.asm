lw   t0,x0,array        # t0 = base address of array
lw   t1,x0, array_len
lw   t2, 0(t1)        # t2 = length (n)
addi t3, x0, 0        # outer loop counter i = 0

    addi t4, t2, -1       # t4 = n - 1
sub  t4, t4, t3       # t4 = n - 1 - i
addi t5, x0, 0        # inner loop counter j = 0