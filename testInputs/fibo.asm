    .text

main:
    addi a0, x0, 10       # a0 = 10 (n)
    jal ra, fib           # call fib(n), result in a0

end:
    jal x0, end           # infinite loop after done

fib:
    # prologue
    addi sp, sp, -16
    sw ra, 12(sp)
    sw s0, 8(sp)
    sw s1, 4(sp)
    sw s2, 0(sp)

    # i = 0
    addi s0, x0, 0

    # a = 0, b = 1
    addi s1, x0, 0
    addi s2, x0, 1

fib_loop:
    beq s0, a0, fib_done

    add t0, s1, s2        # t0 = a + b
    add s1, x0, s2        # a = b
    add s2, x0, t0        # b = t0
    addi s0, s0, 1        # i++

    jal x0, fib_loop

fib_done:
    add a0, x0, s1        # return a in a0

    # epilogue
    lw ra, 12(sp)
    lw s0, 8(sp)
    lw s1, 4(sp)
    lw s2, 0(sp)
    addi sp, sp, 16
    jalr x0, ra, 0
