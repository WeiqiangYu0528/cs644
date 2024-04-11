extern __malloc
extern __exception

section .data
Boolean.MAX_VALUE: dd 0
Integer.MAX_VALUE: dd 0
Byte.MAX_VALUE: dd 0

section .text
global _start
_start:
call J1e_divisionbyzero_test_int
mov ebx, eax
mov eax, 1
int 0x80

System_gc_void:
push ebp
mov ebp, esp
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

J1e_divisionbyzero_test_int:
push ebp
mov ebp, esp
mov ebx, 0
sub esp, 4
mov [ebp-4], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-8], ebx
mov ebx, [ebp-4]
mov ecx, ebx
mov ebx, [ebp-8]
mov eax, ebx
cdq
mov ebx, ecx
idiv ebx
mov ebx, eax
sub esp, 4
mov [ebp-12], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
setg bl
movzx ebx, bl
jg label0

label2:
mov ebx, 123
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
