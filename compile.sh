nasm -O1 -f elf -g -F dwarf output/output.s
ld -melf_i386 -o main output/*.o
./main   
echo $?
