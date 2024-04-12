extern __malloc
extern __exception

section .data
Boolean_MAX_VALUE: dd 127
Integer_MAX_VALUE: dd 2147483647
Byte_MAX_VALUE: dd 127

section .text
global _start
_start:
call J1_sim_or_test_int
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
je label13

label14:
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
je label15

label16:
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
jne label10

label12:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label17:
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
je label20

label21:
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
jl label18
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label13:
call __exception
jmp label14

label15:
call __exception
jmp label16

label10:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label12

label20:
call __exception
jmp label21

label18:
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
je label25
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
jb label27

label25:
call __exception

label27:
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
je label28
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
jb label30

label28:
call __exception

label30:
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
jne label22

label24:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label17

label22:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label24

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
je label34

label35:
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
je label36

label37:
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
jne label31

label33:
mov ebx, 0
sub esp, 4
mov [ebp-32], ebx

label38:
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
je label41

label42:
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
jl label39
mov ebx, 1
mov eax, ebx
mov esp, ebp
pop ebp
ret

label34:
call __exception
jmp label35

label36:
call __exception
jmp label37

label31:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label33

label41:
call __exception
jmp label42

label39:
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
je label46
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
jb label48

label46:
call __exception

label48:
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
je label49
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
jb label51

label49:
call __exception

label51:
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
jne label43

label45:
mov ebx, [ebp-32]
sub esp, 4
mov [ebp-116], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-32], ebx
jmp label38

label43:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label45

J1_sim_or_test_int:
push ebp
mov ebp, esp
mov ebx, 3
sub esp, 4
mov [ebp-4], ebx
mov ebx, 3
mov ecx, ebx
mov ebx, [ebp-4]
imul ebx, ecx
sub esp, 4
mov [ebp-8], ebx
mov ebx, 3
mov ecx, ebx
mov ebx, [ebp-8]
imul ebx, ecx
sub esp, 4
mov [ebp-12], ebx
mov ebx, 1
sub esp, 4
mov [ebp-16], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-16]
imul ebx, ecx
sub esp, 4
mov [ebp-20], ebx
mov ebx, 3
mov ecx, ebx
mov ebx, [ebp-20]
imul ebx, ecx
sub esp, 4
mov [ebp-24], ebx
mov ebx, 4
mov ecx, ebx
mov ebx, [ebp-24]
imul ebx, ecx
sub esp, 4
mov [ebp-28], ebx
mov ebx, 5
mov ecx, ebx
mov ebx, [ebp-28]
imul ebx, ecx
sub esp, 4
mov [ebp-32], ebx
mov ebx, [ebp-32]
push ebx
mov ebx, [ebp-12]
push ebx
call J1_sim_or_sim_or_int_int_int
sub esp, 4
mov [ebp-36], eax
mov ebx, [ebp-36]
mov eax, ebx
mov esp, ebp
pop ebp
ret

J1_sim_or_sim_or_int_int_int:
push ebp
mov ebp, esp
mov [ebp+8], eax
mov ebx, [ebp+8]
sub esp, 4
mov [ebp-4], ebx
mov ebx, [ebp+12]
sub esp, 4
mov [ebp-8], ebx
mov ebx, 0
sub esp, 4
mov [ebp-12], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-16], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-16]
cmp ebx, ecx
sete bl
movzx ebx, bl
je label3

label4:
mov ebx, [ebp-12]
cmp ebx, 1
je label0

label2:
mov ebx, 1
mov [ebp-12], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-20], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-20]
mov eax, ebx
cdq
mov ebx, ecx
idiv ebx
mov ebx, edx
sub esp, 4
mov [ebp-24], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-24]
cmp ebx, ecx
setne bl
movzx ebx, bl
jne label5
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-28], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-28]
mov eax, ebx
cdq
mov ebx, ecx
idiv ebx
mov ebx, edx
sub esp, 4
mov [ebp-32], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-32]
cmp ebx, ecx
setne bl
movzx ebx, bl
mov [ebp-12], ebx

label5:
mov ebx, [ebp-12]
sub esp, 4
mov [ebp-36], ebx
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-40], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-40]
mov eax, ebx
cdq
mov ebx, ecx
idiv ebx
mov ebx, eax
sub esp, 4
mov [ebp-44], ebx
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-48], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-48]
mov eax, ebx
cdq
mov ebx, ecx
idiv ebx
mov ebx, eax
sub esp, 4
mov [ebp-52], ebx
mov ebx, [ebp-52]
push ebx
mov ebx, [ebp-44]
push ebx
call J1_sim_or_sim_or_int_int_int
sub esp, 4
mov [ebp-56], eax
mov ebx, [ebp-56]
sub esp, 4
mov [ebp-60], ebx
mov ebx, 2
mov ecx, ebx
mov ebx, [ebp-60]
imul ebx, ecx
sub esp, 4
mov [ebp-64], ebx
mov ebx, [ebp-36]
cmp ebx, 1
je label7

label9:
mov ebx, [ebp-64]
mov eax, ebx
mov esp, ebp
pop ebp
ret

label3:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-68], ebx
mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-68]
cmp ebx, ecx
sete bl
movzx ebx, bl
mov [ebp-12], ebx
jmp label4

label0:
mov ebx, 0
mov eax, ebx
mov esp, ebp
pop ebp
ret
jmp label2

label7:
mov ebx, [ebp-64]
sub esp, 4
mov [ebp-72], ebx
mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-72]
add ebx, ecx
mov [ebp-64], ebx
jmp label9
