        add 1 0 0      ; R1 = 0
        add 1 1 1      ; R1 = R1 + R1 = 0
        add 0 1 1      ; 写 reg0 = 0+0，但正确应强制保持 0
        beq 0 0 done   ; 如果 reg0≠0，跳转逻辑会错
        halt
done    halt