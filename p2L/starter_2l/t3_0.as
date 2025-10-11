Begin	add	0	0	1
	lw	0	0	Second
	lw	0	0	data
	beq	0	1	done
	beq	0	0	Begin
done	halt
	.fill	0
data	.fill	5
