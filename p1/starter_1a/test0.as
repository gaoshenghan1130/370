        add     7   7   7       # 所有寄存器都是 7
        nor     0   0   0       # 所有寄存器都是 0
        lw      0   1   -32768  # offset 最小
        sw      0   1   32767   # offset 最大
        beq     1   1   0       # offset = 0
        beq     1   2   1       # offset = +1
        beq     2   1   -1      # offset = -1
        jalr    7   7           # 最大寄存器
        halt
        noop
pos     .fill   32767
neg     .fill   -32768
zero    .fill   0
