global _start

section .text

_start:
call A_test
mov ebx, eax
mov eax, 1
int 0x80


A_test:
push ebp
mov ebp, esp
mov ebx, 0
sub esp, 4
mov [ebp-4], ebx

mov ebx, 2
sub esp, 4
mov [ebp-8], ebx

mov ebx, 6
sub esp, 4
mov [ebp-12], ebx

mov ebx, [ebp-12]

push ebx
mov ebx, [ebp-8]

push ebx
call A_add_int_int
sub esp, 4
mov ebx, [ebp-16]

mov eax, ebx
mov esp, ebp
mov pop, ebp
ret
A_add_int_int:
push ebp
mov ebp, esp
mov ebx, [ebp+8]
sub esp, 4
mov [ebp-4], ebx

mov ebx, [ebp+12]
sub esp, 4
mov [ebp-8], ebx

mov ebx, [ebp-4]
sub esp, 4
mov [ebp-12], ebx

mov ebx, [ebp-8]
mov ecx, ebx
mov ebx, [ebp-12]
add ebx, ecx

mov eax, ebx
mov esp, ebp
mov pop, ebp
ret