	lw	0	1	five
	sw	0	1	loc
	lw	0	2	six
	add	1	2	2
	nor	2	2	2
	sw	0	2	loc
	halt
five	.fill	5
six	.fill	6
loc	.fill	0
