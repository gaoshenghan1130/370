	lw	0	1	mem	# reg[1]=mem[7]=12
st	add	0	1	3
	beq	3	2	end	# --- IGNORE ---
	add	0	1	2	// 这一条要被flush，不应该被执行
	halt
end	halt
mem	.fill	12			# mem[7]=12
