extern __malloc
extern __exception

section .data
Boolean_MAX_VALUE: dd 127
Integer_MAX_VALUE: dd 2147483647
Byte_MAX_VALUE: dd 127
J2_fieldinit_forward_ref_j: dd 17
J2_fieldinit_forward_ref_i: dd 1

section .text
global _start
_start:
call J2_fieldinit_forward_ref_test_int
mov ebx, eax
mov eax, 1
int 0x80

System_gc_void:
push ebp
mov ebp, esp
sub esp, 24
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

Arrays_equals_array_boolean_array_boolean_boolean:
push ebp
mov ebp, esp
sub esp, 132
mov ebx, [ebp+8]
mov [ebp-4], ebx
mov ebx, [ebp+12]
mov [ebp-8], ebx
mov ebx, [ebp-4]
mov [ebp-12], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label3

label4:
mov ebx, [ebp-4]
mov [ebp-16], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-16]
sub ebx, ecx
mov ebx, [ebx]
mov [ebp-20], ebx
mov ebx, [ebp-8]
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label5

label6:
mov ebx, [ebp-8]
mov [ebp-28], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-28]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-20]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label0

label2:
mov ebx, 0
mov [ebp-32], ebx

label7:
mov ebx, [ebp-32]
mov [ebp-36], ebx
mov ebx, [ebp-4]
mov [ebp-40], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-40]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label10

label11:
mov ebx, [ebp-4]
mov [ebp-44], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-44]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-36]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label8
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label3:
call __exception
jmp label4

label5:
call __exception
jmp label6

label0:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label2

label10:
call __exception
jmp label11

label8:
mov ebx, [ebp-4]
mov [ebp-48], ebx
mov ebx, [ebp-48]
mov [ebp-52], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-52]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label15
mov ebx, [ebp-32]
mov [ebp-56], ebx
mov ebx, [ebp-56]
mov [ebp-60], ebx
mov ebx, [ebp-48]
mov [ebp-64], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-64]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-60]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label17

label15:
call __exception

label17:
mov ebx, [ebp-48]
mov [ebp-68], ebx
mov ebx, [ebp-56]
mov [ebp-72], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-72]
imul ebx, ecx
mov [ebp-76], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-76]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-68]
add ebx, ecx
mov ebx, [ebx]
mov [ebp-80], ebx
mov ebx, [ebp-8]
mov [ebp-84], ebx
mov ebx, [ebp-84]
mov [ebp-88], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-88]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label18
mov ebx, [ebp-32]
mov [ebp-92], ebx
mov ebx, [ebp-92]
mov [ebp-96], ebx
mov ebx, [ebp-84]
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
jb label20

label18:
call __exception

label20:
mov ebx, [ebp-84]
mov [ebp-104], ebx
mov ebx, [ebp-92]
mov [ebp-108], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-108]
imul ebx, ecx
mov [ebp-112], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-112]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-104]
add ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-80]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label12

label14:
mov ebx, [ebp-32]
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label7

label12:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label14

Arrays_equals_array_char_array_char_boolean:
push ebp
mov ebp, esp
sub esp, 132
mov ebx, [ebp+8]
mov [ebp-4], ebx
mov ebx, [ebp+12]
mov [ebp-8], ebx
mov ebx, [ebp-4]
mov [ebp-12], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label24

label25:
mov ebx, [ebp-4]
mov [ebp-16], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-16]
sub ebx, ecx
mov ebx, [ebx]
mov [ebp-20], ebx
mov ebx, [ebp-8]
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label26

label27:
mov ebx, [ebp-8]
mov [ebp-28], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-28]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-20]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label21

label23:
mov ebx, 0
mov [ebp-32], ebx

label28:
mov ebx, [ebp-32]
mov [ebp-36], ebx
mov ebx, [ebp-4]
mov [ebp-40], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-40]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label31

label32:
mov ebx, [ebp-4]
mov [ebp-44], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-44]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-36]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label29
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label24:
call __exception
jmp label25

label26:
call __exception
jmp label27

label21:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label23

label31:
call __exception
jmp label32

label29:
mov ebx, [ebp-4]
mov [ebp-48], ebx
mov ebx, [ebp-48]
mov [ebp-52], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-52]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label36
mov ebx, [ebp-32]
mov [ebp-56], ebx
mov ebx, [ebp-56]
mov [ebp-60], ebx
mov ebx, [ebp-48]
mov [ebp-64], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-64]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-60]
cmp ebx, ecx
setb bl
movzx ebx, bl
jb label38

label36:
call __exception

label38:
mov ebx, [ebp-48]
mov [ebp-68], ebx
mov ebx, [ebp-56]
mov [ebp-72], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-72]
imul ebx, ecx
mov [ebp-76], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-76]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-68]
add ebx, ecx
mov ebx, [ebx]
mov [ebp-80], ebx
mov ebx, [ebp-8]
mov [ebp-84], ebx
mov ebx, [ebp-84]
mov [ebp-88], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-88]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label39
mov ebx, [ebp-32]
mov [ebp-92], ebx
mov ebx, [ebp-92]
mov [ebp-96], ebx
mov ebx, [ebp-84]
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
jb label41

label39:
call __exception

label41:
mov ebx, [ebp-84]
mov [ebp-104], ebx
mov ebx, [ebp-92]
mov [ebp-108], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-108]
imul ebx, ecx
mov [ebp-112], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-112]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-104]
add ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-80]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label33

label35:
mov ebx, [ebp-32]
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label28

label33:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label35

J2_fieldinit_forward_ref_test_int:
push ebp
mov ebp, esp
sub esp, 28
mov ebx, 122
mov [ebp-4], ebx
mov ebx, [J2_fieldinit_forward_ref_i]
mov ecx, ebx
mov ebx, [ebp-4]
add ebx, ecx
mov eax, ebx
mov esp, ebp
pop ebp
ret

zerodivisionlabel:
call __exception
