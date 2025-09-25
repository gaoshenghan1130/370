	lw	0	2	neg1
subone	add	0	2	1
	jalr	1	0
	halt
neg1	.fill	-1
SubAdr	.fill	subone
