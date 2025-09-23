Main	lw	0	1	LData
	sw	0	1	GData
	add	1	1	2
	jalr	2	0
	halt
LData	.fill	42
GData	.fill	Global
Global	.fill	100
