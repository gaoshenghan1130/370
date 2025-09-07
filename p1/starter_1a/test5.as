        add     7   7   7       
        nor     0   0   0       
        lw      0   1   -32768  
        sw      0   1   32767   
        beq     1   1   0       
        beq     1   19   1       
        beq     2   8   -1      
        jalr    7   7           
        halt
        noop
pos     .fill   32767
neg     .fill   -32768
zero    .fill   0
