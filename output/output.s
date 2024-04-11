section .data
Boolean.MAX_VALUE: dd 127
Integer.MAX_VALUE: dd 2147483647
Byte.MAX_VALUE: dd 127

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

A_m0_int:
push ebp
mov ebp, esp
mov ebx, [ebp+8]
sub esp, 4
mov [ebp-4], ebx
mov ebx, 0
sub esp, 4
mov [ebp-8], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-12], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label0

label2:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-16]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label3

label5:
mov ebx, 1
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
mov ecx, ebx
mov ebx, [ebp-20]
xor ebx, ecx
cmp ebx, 1
je label6

label8:
mov ebx, 1
sub esp, 4
mov [ebp-28], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-32], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-32]
cmp ebx, ecx
setne bl
movzx ebx, bl
mov ecx, ebx
mov ebx, [ebp-28]
xor ebx, ecx
cmp ebx, 1
je label9

label11:
mov ebx, [ebp-8]
mov eax, ebx
mov esp, ebp
pop ebp
ret

label0:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-36], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-36]
add ebx, ecx
mov [ebp-8], ebx
jmp label2

label3:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-40]
add ebx, ecx
mov [ebp-8], ebx
jmp label5

label6:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-44], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-44]
add ebx, ecx
mov [ebp-8], ebx
jmp label8

label9:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-48], ebx
mov ebx, 8
mov ecx, ebx
mov ebx, [ebp-48]
add ebx, ecx
mov [ebp-8], ebx
jmp label11

A_m1_int:
push ebp
mov ebp, esp
mov ebx, [ebp+8]
sub esp, 4
mov [ebp-4], ebx
mov ebx, 0
sub esp, 4
mov [ebp-8], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-12], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label12

label14:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-16]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label15

label17:
mov ebx, 1
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-24], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
mov ecx, ebx
mov ebx, [ebp-20]
xor ebx, ecx
cmp ebx, 1
je label18

label20:
mov ebx, 1
sub esp, 4
mov [ebp-28], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-32], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-32]
cmp ebx, ecx
setne bl
movzx ebx, bl
mov ecx, ebx
mov ebx, [ebp-28]
xor ebx, ecx
cmp ebx, 1
je label21

label23:
mov ebx, [ebp-8]
mov eax, ebx
mov esp, ebp
pop ebp
ret

label12:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-36], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-36]
add ebx, ecx
mov [ebp-8], ebx
jmp label14

label15:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-40]
add ebx, ecx
mov [ebp-8], ebx
jmp label17

label18:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-44], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-44]
add ebx, ecx
mov [ebp-8], ebx
jmp label20

label21:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-48], ebx
mov ebx, 8
mov ecx, ebx
mov ebx, [ebp-48]
add ebx, ecx
mov [ebp-8], ebx
jmp label23

A_test:
push ebp
mov ebp, esp
mov ebx, 0
sub esp, 4
mov [ebp-4], ebx
mov ebx, [ebp-4]
push ebx
call A_m0_int
sub esp, 4
mov [ebp-8], eax
mov ebx, [ebp-8]
mov eax, ebx
mov esp, ebp
pop ebp
ret
