section .data
Boolean.MAX_VALUE: dd 0
Integer.MAX_VALUE: dd 0
Byte.MAX_VALUE: dd 0

section .text
global _start
_start:
call J1_arrayAccess_test
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

J1_arrayAccess_test:
push ebp
mov ebp, esp
mov ebx, 0
sub esp, 4
mov [ebp-4], ebx
mov ebx, 4
sub esp, 4
mov [ebp-8], ebx
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-12], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label0

label1:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-16]
imul ebx, ecx
sub esp, 4
mov [ebp-20], ebx
mov ebx, 8
mov ecx, ebx
mov ebx, [ebp-20]
add ebx, ecx
sub esp, 4
mov [ebp-24], ebx
mov ebx, [ebp-24]
push ebx
call __malloc
sub esp, 4
mov [ebp-28], eax
mov ebx, [ebp-28]
sub esp, 4
mov [ebp-32], ebx
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-36], ebx
mov ebx, [ebp-8]
mov ebx, [ebp-36]
mov ebx, [ebx]
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-40]
add ebx, ecx
mov [ebp-4], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-44], ebx
mov ebx, [ebp-44]
sub esp, 4
mov [ebp-48], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-48]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label3
mov ebx, 0
sub esp, 4
mov [ebp-52], ebx
mov ebx, [ebp-52]
sub esp, 4
mov [ebp-56], ebx
mov ebx, [ebp-44]
sub esp, 4
mov [ebp-60], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-60]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-56]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label5

label3:
call __exception

label0:
call __exception
jmp label1

label5:
mov ebx, 1
sub esp, 4
mov [ebp-64], eax
mov [ebp-64], ebx
mov ebx, [ebp-44]
sub esp, 4
mov [ebp-68], ebx
mov ebx, [ebp-52]
sub esp, 4
mov [ebp-72], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-72]
imul ebx, ecx
sub esp, 4
mov [ebp-76], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-76]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-68]
add ebx, ecx
sub esp, 4
mov [ebp-80], ebx
mov ebx, [ebp-64]
mov ebx, [ebp-80]
mov ebx, [ebx]
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-84], ebx
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-88], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-88]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label6
mov ebx, 1
sub esp, 4
mov [ebp-92], ebx
mov ebx, [ebp-92]
sub esp, 4
mov [ebp-96], ebx
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-100], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-100]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-96]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label8

label6:
call __exception

label8:
mov ebx, 12
sub esp, 4
mov [ebp-104], eax
mov [ebp-104], ebx
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-108], ebx
mov ebx, [ebp-92]
sub esp, 4
mov [ebp-112], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-112]
imul ebx, ecx
sub esp, 4
mov [ebp-116], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-108]
add ebx, ecx
sub esp, 4
mov [ebp-120], ebx
mov ebx, [ebp-104]
mov ebx, [ebp-120]
mov ebx, [ebx]
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-124], ebx
mov ebx, [ebp-124]
sub esp, 4
mov [ebp-128], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-128]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label9
mov ebx, 2
sub esp, 4
mov [ebp-132], ebx
mov ebx, [ebp-132]
sub esp, 4
mov [ebp-136], ebx
mov ebx, [ebp-124]
sub esp, 4
mov [ebp-140], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-140]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-136]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label11

label9:
call __exception

label11:
mov ebx, 123
sub esp, 4
mov [ebp-144], eax
mov [ebp-144], ebx
mov ebx, [ebp-124]
sub esp, 4
mov [ebp-148], ebx
mov ebx, [ebp-132]
sub esp, 4
mov [ebp-152], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-152]
imul ebx, ecx
sub esp, 4
mov [ebp-156], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-156]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-148]
add ebx, ecx
sub esp, 4
mov [ebp-160], ebx
mov ebx, [ebp-144]
mov ebx, [ebp-160]
mov ebx, [ebx]
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-164], ebx
mov ebx, [ebp-164]
sub esp, 4
mov [ebp-168], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-168]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label12
mov ebx, 3
sub esp, 4
mov [ebp-172], ebx
mov ebx, [ebp-172]
sub esp, 4
mov [ebp-176], ebx
mov ebx, [ebp-164]
sub esp, 4
mov [ebp-180], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-180]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-176]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label14

label12:
call __exception

label14:
mov ebx, 1234
sub esp, 4
mov [ebp-184], eax
mov [ebp-184], ebx
mov ebx, [ebp-164]
sub esp, 4
mov [ebp-188], ebx
mov ebx, [ebp-172]
sub esp, 4
mov [ebp-192], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-192]
imul ebx, ecx
sub esp, 4
mov [ebp-196], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-196]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-188]
add ebx, ecx
sub esp, 4
mov [ebp-200], ebx
mov ebx, [ebp-184]
mov ebx, [ebp-200]
mov ebx, [ebx]
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-204], ebx
mov ebx, [ebp-204]
sub esp, 4
mov [ebp-208], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-208]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label15
mov ebx, 2
sub esp, 4
mov [ebp-212], ebx
mov ebx, [ebp-212]
sub esp, 4
mov [ebp-216], ebx
mov ebx, [ebp-204]
sub esp, 4
mov [ebp-220], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-220]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-216]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label17

label15:
call __exception

label17:
mov [ebp-204], eax
mov ebx, [ebp-204]
sub esp, 4
mov [ebp-224], ebx
mov ebx, [ebp-212]
sub esp, 4
mov [ebp-228], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-228]
imul ebx, ecx
sub esp, 4
mov [ebp-232], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-232]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-224]
add ebx, ecx
mov ebx, [ebx]
mov eax, ebx
mov esp, ebp
pop ebp
ret
