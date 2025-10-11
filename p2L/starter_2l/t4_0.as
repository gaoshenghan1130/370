Begin	lw	0	1	five
	lw	0	4	SubAdr
	lw	0	5	Stack
	lw	0	6	data
start	jalr	4	7
	sw	5	0	Stack
	sw	6	0	Stack
	beq	0	1	done
	beq	0	0	start
five	.fill	Three
done	halt
data	.fill	0
