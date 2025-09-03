make


for f in test*.as; do
    ./assembler "$f" "${f%.as}.mc"
done

