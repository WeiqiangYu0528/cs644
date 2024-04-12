extern __malloc
extern __exception

section .data
Integer_MAX_VALUE: dd 2147483647
Boolean_MAX_VALUE: dd 127
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

Arrays_equals_array_boolean_array_boolean_boolean:
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
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label7

label8:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-16]
sub ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label9

label10:
mov ebx, [ebp-8]
sub esp, 4
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
jne label4

label6:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label11:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-36], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-40]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label14

label15:
mov ebx, [ebp-4]
sub esp, 4
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
jl label12
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label7:
call __exception
jmp label8

label9:
call __exception
jmp label10

label4:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label6

label14:
call __exception
jmp label15

label12:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-48], ebx
mov ebx, [ebp-48]
sub esp, 4
mov [ebp-52], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-52]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label19
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-56], ebx
mov ebx, [ebp-56]
sub esp, 4
mov [ebp-60], ebx
mov ebx, [ebp-48]
sub esp, 4
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
jb label21

label19:
call __exception

label21:
mov ebx, [ebp-48]
sub esp, 4
mov [ebp-68], ebx
mov ebx, [ebp-56]
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
mov ebx, [ebx]
sub esp, 4
mov [ebp-80], ebx
mov ebx, [ebp-8]
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
je label22
mov ebx, [ebp-32]
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
jb label24

label22:
call __exception

label24:
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-104], ebx
mov ebx, [ebp-92]
sub esp, 4
mov [ebp-108], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-108]
imul ebx, ecx
sub esp, 4
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
jne label16

label18:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label11

label16:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label18

Arrays_equals_array_char_array_char_boolean:
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
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label28

label29:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-16]
sub ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label30

label31:
mov ebx, [ebp-8]
sub esp, 4
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
jne label25

label27:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label32:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-36], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-40]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label35

label36:
mov ebx, [ebp-4]
sub esp, 4
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
jl label33
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label28:
call __exception
jmp label29

label30:
call __exception
jmp label31

label25:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label27

label35:
call __exception
jmp label36

label33:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-48], ebx
mov ebx, [ebp-48]
sub esp, 4
mov [ebp-52], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-52]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label40
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-56], ebx
mov ebx, [ebp-56]
sub esp, 4
mov [ebp-60], ebx
mov ebx, [ebp-48]
sub esp, 4
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
jb label42

label40:
call __exception

label42:
mov ebx, [ebp-48]
sub esp, 4
mov [ebp-68], ebx
mov ebx, [ebp-56]
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
mov ebx, [ebx]
sub esp, 4
mov [ebp-80], ebx
mov ebx, [ebp-8]
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
je label43
mov ebx, [ebp-32]
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
jb label45

label43:
call __exception

label45:
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-104], ebx
mov ebx, [ebp-92]
sub esp, 4
mov [ebp-108], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-108]
imul ebx, ecx
sub esp, 4
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
jne label37

label39:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label32

label37:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label39

System_gc_void:
push ebp
mov ebp, esp
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

J2_fieldinit_forward_ref_test_int:
push ebp
mov ebp, esp
mov ebx, 122
sub esp, 4
mov [ebp-4], ebx
sub esp, 4
mov ebx, [J2_fieldinit_forward_ref_i]
mov ecx, ebx
mov ebx, [ebp-4]
add ebx, ecx
mov eax, ebx
mov esp, ebp
pop ebp
ret
