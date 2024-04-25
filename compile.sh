for file in output/*.s; do
    nasm -O1 -f elf -g -F dwarf "$file"
done
ld -melf_i386 -o main output/*.o
./main
echo $?
rm -rf output/*
cp JSL_6.1/runtime.o output/
cp JSL_6.1/runtime.s output/
