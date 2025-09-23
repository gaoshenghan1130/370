start	lw	0	1	data1
	lw	0	2	data2
	add	1	2	3
	nor	3	1	4
	beq	0	0	loop
	sw	0	3	Aaa
	jalr	4	7
	beq	0	0	end
loop	lw	0	5	eData
	sw	0	5	lData
	add	5	5	5
	beq	0	0	Loop2
loop2	lw	0	6	lData2
	sw	0	6	eData2
end	halt
data1	.fill	10
data2	.fill	20
data3	.fill	30
lData	.fill	100
lData2	.fill	200
eData	.fill	0
eData2	.fill	0
