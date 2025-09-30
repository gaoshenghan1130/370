        lw          0       1       five       ; $1 = 5
        lw          0       4       SubAdr     ; Store address of SubAdr
start   jalr        4       7                  ; Store return address and jump to SubAdr
        beq         0       1       done       ; Finish if $1 == 0
        beq         0       0       start      ; Otherwise continue (keep calling SubAdr).
done    halt
five    .fill       5
Global  .fill       Globex
Globex  .fill       Global
