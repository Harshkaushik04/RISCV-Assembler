.data
array:       .word 8, 300, 6, 5, 4, 3, 2, 1     # 8 integers, 32 bytes total
array_len:   .word 8                           # length = 8
aux_buffer:  .word 0,0,0,0,0,0,0,0             # aux buffer, 8 words (32 bytes)
new:         .asciz "Hello"
.text
_start:
    # Load array address and length
    lw   t2, 0(t1)         # t2 = length (n)

    # Call merge_sort(arr, 0, n-1)
    addi t3, x0, 0         # low = 0
    addi t4, t2, -1        # high = n-1
    jal  ra, _merge_sort

# void merge_sort(int *arr, int low, int high)
_merge_sort:
    addi sp, sp, -16
    sw   ra, 12(sp)
    sw   s0, 8(sp)
    sw   s1, 4(sp)
    sw   s2, 0(sp)

    blt  s2, s1, merge_sort_end     # if high < low, return

    beq  s1, s2, merge_sort_end     # if low == high, return

    add  t0, s1, s2
    srai t0, t0, 1                  # mid = (low + high) / 2

    # merge_sort(arr, low, mid)
    jal  ra, _merge_sort

    # merge_sort(arr, mid+1, high)
    addi a1, t0, 1
    jal  ra, _merge_sort

    # merge(arr, low, mid, high)
    jal  ra, _merge

merge_sort_end:
    lw   ra, 12(sp)
    lw   s0, 8(sp)
    lw   s1, 4(sp)
    lw   s2, 0(sp)
    addi sp, sp, 16

# void merge(int *arr, int low, int mid, int high)
_merge:
    addi sp, sp, -24
    sw   ra, 20(sp)
    sw   s0, 16(sp)
    sw   s1, 12(sp)
    sw   s2, 8(sp)
    sw   s3, 4(sp)
    sw   s4, 0(sp)


    addi t0, x0, 0      # i = low
    addi t1, x0, 0      # j = mid + 1
    addi t1, s2, 1

merge_loop:
    bgt  t0, s2, copy_right_loop   # if i > mid, copy rest of right half
    bgt  t1, s3, copy_left_loop    # if j > high, copy rest of left half

    slli t3, t0, 2                 # offset i*4
    add  t4, s0, t3
    lw   t5, 0(t4)                 # arr[i]

    slli t6, t1, 2                 # offset j*4
    add  t7, s0, t6
    lw   t8, 0(t7)                 # arr[j]

    blt  t5, t8, take_left
    # take right
    sw   t8, 0(t2)                 # aux_buffer[k] = arr[j]
    addi t1, t1, 1
    addi t2, t2, 4

take_left:
    sw   t5, 0(t2)                 # aux_buffer[k] = arr[i]
    addi t0, t0, 1
    addi t2, t2, 4

copy_left_loop:
    ble  t0, s2, copy_left_continue

copy_left_continue:
    slli t3, t0, 2
    add  t4, s0, t3
    lw   t5, 0(t4)
    sw   t5, 0(t2)
    addi t0, t0, 1
    addi t2, t2, 4

copy_right_loop:
    ble  t1, s3, copy_right_continue

copy_right_continue:
    slli t6, t1, 2
    add  t7, s0, t6
    lw   t8, 0(t7)
    sw   t8, 0(t2)
    addi t1, t1, 1
    addi t2, t2, 4

copy_back_loop:
    addi t0, s1, 0
copy_back_index_loop:
    bgt  t0, s3, copy_back_done
    slli t1, t0, 2
    add  t2, s0, t1     # arr + offset
    slli t3, t0, 2
    sub  t4, t0, s1
    slli t4, t4, 2
    add  t5, t9, t4     # aux_buffer + (t0 - low)*4
    lw   t6, 0(t5)
    sw   t6, 0(t2)
    addi t0, t0, 1

copy_back_done:
    lw   ra, 20(sp)
    lw   s0, 16(sp)
    lw   s1, 12(sp)
    lw   s2, 8(sp)
    lw   s3, 4(sp)
    lw   s4, 0(sp)
    addi sp, sp, 24
