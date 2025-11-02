make
make assembler
./assembler test1.as test1.mc
./simulator test1.mc > test1.out

./assembler p3spec.as p3spec.mc
./assembler test2.as test2.mc
./simulator p3spec.mc > p3spec.out
./simulator test2.mc > test2.out