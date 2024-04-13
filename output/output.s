extern __malloc
extern __exception

section .data
Integer_MAX_VALUE: dd 2147483647
Boolean_MAX_VALUE: dd 127
Byte_MAX_VALUE: dd 127

section .text
global _start
_start:
call A_test_int
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
je label18

label19:
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
je label20

label21:
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
jne label15

label17:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label22:
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
je label25

label26:
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
jl label23
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label18:
call __exception
jmp label19

label20:
call __exception
jmp label21

label15:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label17

label25:
call __exception
jmp label26

label23:
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
je label30
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
jb label32

label30:
call __exception

label32:
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
je label33
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
jb label35

label33:
call __exception

label35:
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
jne label27

label29:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label22

label27:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label29

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
je label39

label40:
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
je label41

label42:
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
jne label36

label38:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label43:
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
je label46

label47:
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
jl label44
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label39:
call __exception
jmp label40

label41:
call __exception
jmp label42

label36:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label38

label46:
call __exception
jmp label47

label44:
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
je label51
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
jb label53

label51:
call __exception

label53:
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
je label54
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
jb label56

label54:
call __exception

label56:
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
jne label48

label50:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label43

label48:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label50

System_gc_void:
push ebp
mov ebp, esp
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret

A_test_int:
push ebp
mov ebp, esp
mov ebx, 1
sub esp, 4
mov [ebp-4], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-8], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-8]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label0

label1:
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-12], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-12]
imul ebx, ecx
sub esp, 4
mov [ebp-16], ebx
mov ebx, 8
mov ecx, ebx
mov ebx, [ebp-16]
add ebx, ecx
sub esp, 4
mov [ebp-20], ebx
mov ebx, [ebp-20]
mov eax, ebx
call __malloc
sub esp, 4
mov [ebp-24], eax
mov ebx, [ebp-24]
sub esp, 4
mov [ebp-28], ebx
mov ebx, [ebp-28]
sub esp, 4
mov [ebp-32], ebx
mov ebx, [ebp-4]
mov ecx, ebx
mov ebx, [ebp-32]
mov [ebx], ecx
mov ebx, [ebp-28]
sub esp, 4
mov [ebp-36], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-36]
add ebx, ecx
sub esp, 4
mov [ebp-40], ebx
mov ebx, [ebp-40]
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
mov ecx, ebx
mov ebx, [ebp-80]
mov [ebx], ecx
mov ebx, 1
sub esp, 4
mov [ebp-84], ebx
mov ebx, 0
sub esp, 4
mov [ebp-88], ebx
mov ebx, 1
sub esp, 4
mov [ebp-92], ebx
mov ebx, 3
mov ecx, ebx
mov ebx, [ebp-92]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label9

label10:
mov ebx, [ebp-88]
cmp ebx, 1
je label6

label8:
mov ebx, 200
mov eax, ebx
mov esp, ebp
pop ebp
ret

label9:
mov ebx, [ebp-84]
sub esp, 4
mov [ebp-96], ebx
mov ebx, 5
mov ecx, ebx
mov ebx, [ebp-96]
cmp ebx, ecx
setg bl
movzx ebx, bl
mov [ebp-88], ebx
jmp label10

label6:
mov ebx, 123
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label8

zerodivisionlabel:
call __exception
