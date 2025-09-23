        add 1 0 1       ; R1 = 0
        add 2 0 2       ; R2 = 0
loop    beq 1 2 end    ; offset = 1 → 跳到 end
        add 1 1 1       ; 不应执行
        beq 1 2 loop    ; offset = -2 → 循环回 loop
end     halt