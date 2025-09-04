        add     1   2   3
        nor     3   4   5
start   lw      0   1   data
        sw      0   1   data
        beq     1   1   end
        beq     1   2   -1
        jalr    4   7
        halt
end     noop
data    .fill   42