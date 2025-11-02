make
make assembler
./assembler test1.as test1.mc
./simulator test1.mc > test1.out
./assembler test3.as test3.mc
./simulator test3.mc > test3.out
./assembler test4.as test4.mc
./simulator test4.mc > test4.out
./assembler test5.as test5.mc
./simulator test5.mc > test5.out


./assembler p3spec.as p3spec.mc
./assembler test2.as test2.mc
./simulator p3spec.mc > p3spec.out
./simulator test2.mc > test2.out