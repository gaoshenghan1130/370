Main	lw	0	1	five	// n = 5
	lw	0	2	three	// r = 3
	lw	0	4	Faddr	// load function address
	jalr	4	7		// call function
	halt				// end of program
five	.fill	5
three	.fill	3
