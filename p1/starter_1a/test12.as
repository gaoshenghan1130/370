        lw 0 1 value    ; R1 = mem[value] → target
        lw 1 2 0        ; R2 = mem[R1 + 0] = 42
        halt
value   .fill target
target  .fill 42