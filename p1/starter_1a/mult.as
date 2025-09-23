	lw	0	1	mcand	; R1 = cand
	lw	0	2	mplier	; R2 = mplier
	lw	0	4	one	; R4 is the mask, initially 1 , then 10, 100, ...
	lw	0	6	sixt	; R6 = counter, initially 16
	add	3	0	0	; R3 = 0 (product)
loop	beq	6	0	done	; if R6 == 0, done, over flow
	nor	2	2	5	; R5 = not mplier
	nor	4	4	7	; R7 = not R4	check the bit of mplier on mask
	nor	5	7	5	; R5 = mplier & R4
	beq	5	0	notad	; if R5 == 1, time 2^ (mask bit)
	add	3	1	3	; product += cand
notad	add	4	4	4	; R4 mask shift left
	lw	0	7	neg1	; R7 = -1
	add	7	6	6	; R6 --
	add	1	1	1	; R1 = R1 * 2
	beq	0	0	loop	; repeat
done	halt
neg1	.fill	-1			; constant -1
sixt	.fill	16			; constant8
one	.fill	1			; constant1
mcand	.fill	6
mplier	.fill	5
