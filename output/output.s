extern __malloc
extern __exception

section .data

section .text
global _start
_start:
call J1_SmallInt_test_int
mov ebx, eax
mov eax, 1
int 0x80

J1_SmallInt_test_int:
push ebp
mov ebp, esp
mov ebx, -1
sub esp, 4
mov [ebp-4], ebx
mov ebx, -2147483648
mov ecx, ebx
mov ebx, [ebp-4]
imul ebx, ecx
sub esp, 4
mov [ebp-8], ebx
mov ebx, 2147483000
mov ecx, ebx
mov ebx, [ebp-8]
add ebx, ecx
sub esp, 4
mov [ebp-12], ebx
mov ebx, 771
mov ecx, ebx
mov ebx, [ebp-12]
add ebx, ecx
mov eax, ebx
mov esp, ebp
pop ebp
ret

J1_SmallInt:
push ebp
mov ebp, esp
mov ebx, 4
sub esp, 4
mov [ebp-4], ebx
mov ebx, [ebp-4]
mov eax, ebx
call __malloc
sub esp, 4
mov [ebp-8], eax
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-12], ebx
mov ebx, [ebp-12]
mov eax, ebx
mov esp, ebp
pop ebp
ret

zerodivisionlabel:
call __exception
