make linker
make assembler

./assembler t0_1.as t0_1.obj
./assembler t0_2.as t0_2.obj
./linker t0_1.obj t0_2.obj t0.obj

./linker count5_0.obj.correct count5_1.obj.correct count5.obj
