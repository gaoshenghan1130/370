    lw   0 1 A
    lw   0 2 B
    add  1 2 3
    beq  3 0 skip
    noop
skip halt
A .fill -7
B .fill 7