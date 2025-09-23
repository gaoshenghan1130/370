Entry	add	0	0	1
	beq	1	0	Skip
	lw	0	2	localA
	sw	0	2	GData
	jalr	3	0
Skip	add	1	2	3
	beq	0	0	Entry
localA	.fill	7
GData	.fill	GlobaX
GlobaX	.fill	9
