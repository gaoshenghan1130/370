make

./assembler main.as main.o

./assembler subone.as subone.o


for i in {0..7}; do
    echo "----------------------"
    echo "Running test$i.as..."
    ./assembler test$i.as test$i.o
    echo "----------------------"
    echo " "
done