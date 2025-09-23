        add 1 0 1       ; R1 = 0
        add 2 0 2       ; R2 = 0
        add 3 0 3       ; R3 = 0
        lw 1 2 data     ; R2 = mem[R1 + 0] = data
        sw 2 3 1        ; mem[R3 + 1] = R2
        halt
data    .fill 42