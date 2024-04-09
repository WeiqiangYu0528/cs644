nasm -O1 -f elf -g -F dwarf test.s
ld -melf_i386 -o main test.o 
./main   
echo $?