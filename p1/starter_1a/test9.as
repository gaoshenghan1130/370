	lw	0	1	val	; R1 = 99
	lw	0	2	pos	; R2 = 5
	sw	1	2	0	; mem[5] = 99
loop	lw	0	3	five	; R3 = mem[5] (应为 99)
	halt
val	.fill	99
pos	.fill	5
five	.fill	0
