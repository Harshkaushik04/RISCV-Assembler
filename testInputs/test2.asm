.data
array:      .word 5, 3, 2, 4, 1     # unsorted array
array_len:  .word 5

.text
_start:
    # Load array address and length
    la   t0, array        # t0 = base address of array
    la   t1, array_len
    lw   t2, 0(t1)        # t2 = length (n)

    addi t3, x0, 0        # outer loop counter i = 0
outer_loop:
    addi t4, t2, -1       # t4 = n - 1
    sub  t4, t4, t3       # t4 = n - 1 - i
    addi t5, x0, 0        # inner loop counter j = 0

inner_loop:
    bge  t5, t4, end_inner

    slli t6, t5, 2        # t6 = j * 4 (offset in bytes)
    add  t7, t0, t6       # t7 = &array[j]
    lw   t8, 0(t7)        # t8 = array[j]
    lw   t9, 4(t7)        # t9 = array[j+1]

    blt  t8, t9, skip_swap

    # Swap array[j] and array[j+1]
    sw   t9, 0(t7)
    sw   t8, 4(t7)

skip_swap:
    addi t5, t5, 1
    j inner_loop

end_inner:
    addi t3, t3, 1
    blt  t3, t2, outer_loop

    # Exit (terminate)
    li a7, 10
