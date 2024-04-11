section .data
Boolean.MAX_VALUE: dd 127
Integer.MAX_VALUE: dd 2147483647
Byte.MAX_VALUE: dd 127

section .text
global _start
_start:
call J1_A_GreaterOrEqual_test
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

J1_A_GreaterOrEqual_test:
push ebp
mov ebp, esp
mov ebx, 1
sub esp, 4
mov [ebp-4], ebx
mov ebx, 2
sub esp, 4
mov [ebp-8], ebx
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-12], ebx
mov ebx, [ebp-4]
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
setge bl
movzx ebx, bl
sub esp, 4
mov [ebp-16], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-4]
mov ecx, ebx
mov ebx, [ebp-20]
cmp ebx, ecx
setge bl
movzx ebx, bl
sub esp, 4
mov [ebp-24], ebx
mov ebx, [ebp-16]
sub esp, 4
mov [ebp-28], ebx
mov ebx, [ebp-24]
mov ecx, ebx
mov ebx, [ebp-28]
and ebx, ecx
sub esp, 4
mov [ebp-32], ebx
mov ebx, [ebp-32]
cmp ebx, 1
je label0

label2:
mov ebx, 42
mov eax, ebx
mov esp, ebp
pop ebp
ret

label0:
mov ebx, 123
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label2
