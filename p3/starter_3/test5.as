	lw	0	1	mem	# reg[1]=mem[7]=12
	lw	0	2	answer	# reg[2]=48
st	add	1	1	3	#
	beq	3	2	end	# reg[2]=24, reg[1]=12, not equal: branch not taken
	beq	0	0	st	# loops back
	add	0	0	7	# runs because branch not taken
end	halt
	halt
mem	.fill	12			# mem[7]=12
answer	.fill	48			# expected answer in reg[2]
