	lw	0	1	five	// n = 5
	lw	0	2	three	// r = 3
	lw	0	4	Start
	jalr	4	7
	halt
start	lw	0	6	one
	beq	2	0	done1	// if r == 0, return 1
	beq	1	2	done1	// if n == r, return 1
	sw	5	7	Stack	// push return addr, n, r
	add	5	6	5
	sw	5	1	Stack
	add	5	6	5
	sw	5	2	Stack
	add	5	6	5
	lw	0	6	neg1	// compute C(n-1, r)
	add	1	6	1
	lw	0	4	Start
	jalr	4	7
	lw	0	6	neg1	// restore r, n
	add	5	6	5
	lw	5	2	Stack
	lw	0	6	neg1
	add	5	6	5
	lw	5	1	Stack
	lw	0	6	one
	sw	5	3	Stack	// save C(n-1, r)
	add	5	6	5
	lw	0	6	neg1	y// compute C(n-1, r-1)
	add	1	6	1
	add	2	6	2
	lw	0	4	Start
	jalr	4	7
	lw	0	6	neg1	// combine results
	add	5	6	5
	lw	5	4	Stack
	add	3	4	3
	add	5	6	5	// pop return addr, (n, r is already popped)
	lw	5	7	Stack
	jalr	7	4
done1	lw	0	6	one
	add	0	6	3
	jalr	7	4
	halt
Faddr	.fill	start
zero	.fill	0
neg1	.fill	-1
one	.fill	1
two	.fill	2
three	.fill	4
five	.fill	5
Start	.fill	start
Stack	.fill	51
