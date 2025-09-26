	lw	0	1	five
	lw	0	4	SubAdr
	jalr	4	7
	beq	0	1	1
	beq	0	0	-3
	halt
five	.fill	5
