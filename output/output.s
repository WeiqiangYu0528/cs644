extern __malloc
extern __exception

section .data
Boolean_MAX_VALUE: dd 127
Integer_MAX_VALUE: dd 2147483647
Byte_MAX_VALUE: dd 127

section .text
global _start
_start:
call A_test_int
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
je label8

label9:
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
je label10

label11:
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
jne label5

label7:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label12:
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
je label15

label16:
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
jl label13
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label8:
call __exception
jmp label9

label10:
call __exception
jmp label11

label5:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label7

label15:
call __exception
jmp label16

label13:
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
je label20
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
jb label22

label20:
call __exception

label22:
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
je label23
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
jb label25

label23:
call __exception

label25:
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
jne label17

label19:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label12

label17:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label19

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
je label29

label30:
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
je label31

label32:
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
jne label26

label28:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label33:
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
je label36

label37:
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
jl label34
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label29:
call __exception
jmp label30

label31:
call __exception
jmp label32

label26:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label28

label36:
call __exception
jmp label37

label34:
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
je label41
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
jb label43

label41:
call __exception

label43:
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
je label44
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
jb label46

label44:
call __exception

label46:
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
jne label38

label40:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label33

label38:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label40

A_test_int:
push ebp
mov ebp, esp
mov ebx, 0
sub esp, 4
mov [ebp-4], ebx
mov ebx, 0
sub esp, 4
mov [ebp-8], ebx
mov ebx, 0
sub esp, 4
mov [ebp-12], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-12]
cmp ebx, ecx
setl bl
movzx ebx, bl
jl label3

label4:
mov ebx, [ebp-8]
cmp ebx, 1
je label0

label2:
mov ebx, [ebp-4]
mov eax, ebx
mov esp, ebp
pop ebp
ret

label3:
mov ebx, 5
sub esp, 4
mov [ebp-16], ebx
mov ebx, 7
mov ecx, ebx
mov ebx, [ebp-16]
cmp ebx, ecx
setg bl
movzx ebx, bl
mov [ebp-8], ebx
jmp label4

label0:
mov ebx, 10
mov [ebp-4], ebx
jmp label2
