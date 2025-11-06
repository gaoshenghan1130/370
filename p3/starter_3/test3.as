	lw	0	1	aaa	# reg[1] = mem[7] (should get 42)
	add	1	1	1	# reg[1] = reg[1] + reg[1] = 84 (should stall)
	sw	0	1	bbb	# mem[8] = reg[1] (should be 84)
	halt
	noop
aaa	.fill	42			# mem[7]=42
bbb	.fill	0
