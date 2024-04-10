section .data
Byte.MAX_VALUE: dd 127
Boolean.MAX_VALUE: dd 127
Integer.MAX_VALUE: dd 2147483647

section .text
global _start
_start:
call A_test
mov ebx, eax
mov eax, 1
int 0x80

System_gc:
push ebp
mov ebp, esp
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

A_test:
push ebp
mov ebp, esp
mov ebx, 1
cmp ebx, 0
jne label0

label2:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

label0:
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label2
