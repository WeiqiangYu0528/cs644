extern __malloc
extern __exception

section .data
A_vtable: dd 

section .text
global _start
_start:
call A_test_int
mov ebx, eax
mov eax, 1
int 0x80

global A_test_int
A_test_int:
push ebp
mov ebp, esp
sub esp, 152
mov ebx, 4
mov [ebp-4], ebx
mov ebx, [ebp-4]
mov [ebp-8], ebx
mov ecx, 0
mov ebx, [ebp-8]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label0

label1:
mov ebx, [ebp-4]
mov [ebp-12], ebx
mov ecx, 4
mov ebx, [ebp-12]
imul ebx, ecx
mov ebx, ebx
mov [ebp-16], ebx
mov ecx, 8
mov ebx, [ebp-16]
add ebx, ecx
mov ebx, ebx
mov [ebp-20], ebx
mov ebx, __malloc
mov [ebp-24], ebx
mov eax, [ebp-20]
call [ebp-24]
mov [ebp-28], eax
mov ebx, [ebp-28]
mov [ebp-32], ebx
mov ebx, [ebp-32]
mov [ebp-36], ebx
mov ebx, [ebp-4]
mov ecx, ebx
mov ebx, [ebp-36]
mov [ebx], ecx
mov ebx, [ebp-32]
mov [ebp-40], ebx
mov ecx, 4
mov ebx, [ebp-40]
add ebx, ecx
mov ebx, ebx
mov [ebp-44], ebx
mov ebx, [ebp-44]
mov [ebp-48], ebx
mov ebx, [ebp-48]
mov [ebp-52], ebx
mov ecx, 0
mov ebx, [ebp-52]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label3
mov ebx, 0
mov [ebp-56], ebx
mov ebx, [ebp-56]
mov [ebp-60], ebx
mov ebx, [ebp-48]
mov [ebp-64], ebx
mov ecx, 4
mov ebx, [ebp-64]
sub ebx, ecx
mov ecx, ebx
mov ebx, [ebp-60]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label5

label3:
mov ebx, __exception
mov [ebp-68], ebx
call [ebp-68]

label0:
mov ebx, __exception
mov [ebp-72], ebx
call [ebp-72]
jmp label1

label5:
mov ebx, 1
mov [ebp-76], ebx
mov ebx, [ebp-48]
mov [ebp-80], ebx
mov ebx, [ebp-56]
mov [ebp-84], ebx
mov ecx, 4
mov ebx, [ebp-84]
imul ebx, ecx
mov ebx, ebx
mov [ebp-88], ebx
mov ecx, 4
mov ebx, [ebp-88]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-80]
add ebx, ecx
mov ebx, ebx
mov [ebp-92], ebx
mov ebx, [ebp-76]
mov ecx, ebx
mov ebx, [ebp-92]
mov [ebx], ecx
mov ebx, [ebp-44]
mov [ebp-96], ebx
mov ebx, [ebp-96]
mov [ebp-100], ebx
mov ecx, 0
mov ebx, [ebp-100]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label6
mov ebx, 0
mov [ebp-104], ebx
mov ebx, [ebp-104]
mov [ebp-108], ebx
mov ebx, [ebp-96]
mov [ebp-112], ebx
mov ecx, 4
mov ebx, [ebp-112]
sub ebx, ecx
mov ecx, ebx
mov ebx, [ebp-108]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label8

label6:
mov ebx, __exception
mov [ebp-116], ebx
call [ebp-116]

label8:
mov ebx, [ebp-96]
mov [ebp-120], ebx
mov ebx, [ebp-104]
mov [ebp-124], ebx
mov ecx, 4
mov ebx, [ebp-124]
imul ebx, ecx
mov ebx, ebx
mov [ebp-128], ebx
mov ecx, 4
mov ebx, [ebp-128]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-120]
add ebx, ecx
mov eax, ebx
mov esp, ebp
pop ebp
ret

global Constructor_A
Constructor_A:
push ebp
mov ebp, esp
sub esp, 28
mov ebx, 4
mov [ebp-4], ebx
mov ebx, __malloc
mov [ebp-8], ebx
mov eax, [ebp-4]
call [ebp-8]
mov [ebp-12], eax
mov ebx, [ebp-12]
mov [ebp-16], ebx
mov ebx, [ebp-16]
mov [ebp-20], ebx
mov ebx, A_vtable
mov ecx, ebx
mov ebx, [ebp-20]
mov [ebx], ecx
mov eax, [ebp-16]
mov esp, ebp
pop ebp
ret

zerodivisionlabel:
call __exception
