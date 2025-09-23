        add 1 0 0
        add 2 0 0
        beq 1 2 neg    ; offset 是 -1，应该跳回去，形成死循环
        halt
neg     halt