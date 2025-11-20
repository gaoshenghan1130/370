gcc cache.c my_p1s_sim.c -lm -o simulator

./assembler t.1.1.1.as t.1.1.1.mc
./simulator t.1.1.1.mc 1 1 1 > t.1.1.1.simout  