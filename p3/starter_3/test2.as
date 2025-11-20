	lw	0	1	one
	lw	0	2	neg1
loop	add	1	2	1
	beq	0	1	end
	beq	0	0	loop
end	halt
one	.fill	1
neg1	.fill	-1
