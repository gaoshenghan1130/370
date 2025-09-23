        add 1 0 0       ; R1 = 0
        add 2 0 0       ; R2 = 0
loop    beq 1 2 end    ; offset = 1 -> should skip next add if R1==R2
        add 1 1 1       ; R1 += R1 (never executed)
        beq 1 2 loop    ; offset = -2 (signed) -> jump back to loop
end     halt