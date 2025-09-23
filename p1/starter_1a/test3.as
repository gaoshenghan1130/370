        lw 0 1 data       ; load data into R1
        add 1 0 1         ; R1 += 0, just to use R1
        halt
data    .fill target
target  .fill 42