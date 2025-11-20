	lw	0	1	A	# R1 = A
	add	1	1	2	# R2 = R1 + R1           (RAW: R1 -> R2)
	add	2	1	3	# R3 = R2 + R1           (RAW: R2 -> R3)
	nor	3	2	4	# R4 = ~(R3 | R2)        (RAW: R3 -> R4)
	lw	0	5	Zero	# R5 = Zero
	halt
A	.fill	30
B	.fill	0			# 分支控制测试
Zero	.fill	0
X	.fill	0
Y	.fill	0
Z	.fill	0
W	.fill	0
V	.fill	0
U	.fill	0
S	.fill	0
temp	.fill	0
