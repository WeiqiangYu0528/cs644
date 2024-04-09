section .data
Byte.MAX_VALUE: dd 127
Integer.MAX_VALUE: dd 2147483647
Boolean.MAX_VALUE: dd 127
A.myStaticMember: dd 0
section .text
global _start
_start:
sub esp, 4
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-8], ebx

sub esp, 4
mov ebx, [ebp-12]
sub esp, 4
mov [ebp-16], ebx

mov ebx, [ebp-8]
sub esp, 4
mov [ebp-20], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-20]
cmp ebx, ecx
, 0
jne label7
label8:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-24], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-24]
sub ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-28], ebx

mov ebx, [ebp-16]
sub esp, 4
mov [ebp-32], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-32]
cmp ebx, ecx
, 0
jne label9
label10:
mov ebx, [ebp-16]
sub esp, 4
mov [ebp-36], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-36]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-28]
cmp ebx, ecx
, 0
jne label4
label6:
mov ebx, 0
sub esp, 4
mov [ebp-40], ebx

label11:
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-44], ebx

mov ebx, [ebp-8]
sub esp, 4
mov [ebp-48], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-48]
cmp ebx, ecx
, 0
jne label14
label15:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-52], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-52]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-44]
cmp ebx, ecx
, 0
jne label12
mov ebx, 1

mov eax, ebx
mov esp, ebp
mov pop ebp
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
mov pop ebp
ret
jmp label6
label14:
call __exception
jmp label15
label12:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-56], ebx

mov ebx, [ebp-56]
sub esp, 4
mov [ebp-60], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-60]
cmp ebx, ecx
, 0
jne label19
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-64], ebx

mov ebx, [ebp-64]
sub esp, 4
mov [ebp-68], ebx

mov ebx, [ebp-56]
sub esp, 4
mov [ebp-72], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-72]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-68]
cmp ebx, ecx
, 0
jne label21
label19:
call __exception
label21:
mov ebx, [ebp-56]
sub esp, 4
mov [ebp-76], ebx

mov ebx, [ebp-64]
sub esp, 4
mov [ebp-80], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-80]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-76]
add ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-84], ebx

mov ebx, [ebp-16]
sub esp, 4
mov [ebp-88], ebx

mov ebx, [ebp-88]
sub esp, 4
mov [ebp-92], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-92]
cmp ebx, ecx
, 0
jne label22
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-96], ebx

mov ebx, [ebp-96]
sub esp, 4
mov [ebp-100], ebx

mov ebx, [ebp-88]
sub esp, 4
mov [ebp-104], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-104]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-100]
cmp ebx, ecx
, 0
jne label24
label22:
call __exception
label24:
mov ebx, [ebp-88]
sub esp, 4
mov [ebp-108], ebx

mov ebx, [ebp-96]
sub esp, 4
mov [ebp-112], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-112]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-108]
add ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-84]
cmp ebx, ecx
, 0
jne label16
label18:
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-116], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-40], ebx

jmp label11
label16:
mov ebx, 0

mov eax, ebx
mov esp, ebp
mov pop ebp
ret
jmp label18
Arrays_equals_array_char_array_char:
sub esp, 4
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-8], ebx

sub esp, 4
mov ebx, [ebp-12]
sub esp, 4
mov [ebp-16], ebx

mov ebx, [ebp-8]
sub esp, 4
mov [ebp-20], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-20]
cmp ebx, ecx
, 0
jne label28
label29:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-24], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-24]
sub ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-28], ebx

mov ebx, [ebp-16]
sub esp, 4
mov [ebp-32], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-32]
cmp ebx, ecx
, 0
jne label30
label31:
mov ebx, [ebp-16]
sub esp, 4
mov [ebp-36], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-36]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-28]
cmp ebx, ecx
, 0
jne label25
label27:
mov ebx, 0
sub esp, 4
mov [ebp-40], ebx

label32:
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-44], ebx

mov ebx, [ebp-8]
sub esp, 4
mov [ebp-48], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-48]
cmp ebx, ecx
, 0
jne label35
label36:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-52], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-52]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-44]
cmp ebx, ecx
, 0
jne label33
mov ebx, 1

mov eax, ebx
mov esp, ebp
mov pop ebp
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
mov pop ebp
ret
jmp label27
label35:
call __exception
jmp label36
label33:
mov ebx, [ebp-8]
sub esp, 4
mov [ebp-56], ebx

mov ebx, [ebp-56]
sub esp, 4
mov [ebp-60], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-60]
cmp ebx, ecx
, 0
jne label40
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-64], ebx

mov ebx, [ebp-64]
sub esp, 4
mov [ebp-68], ebx

mov ebx, [ebp-56]
sub esp, 4
mov [ebp-72], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-72]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-68]
cmp ebx, ecx
, 0
jne label42
label40:
call __exception
label42:
mov ebx, [ebp-56]
sub esp, 4
mov [ebp-76], ebx

mov ebx, [ebp-64]
sub esp, 4
mov [ebp-80], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-80]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-76]
add ebx, ecx
mov ebx, [ebx]
sub esp, 4
mov [ebp-84], ebx

mov ebx, [ebp-16]
sub esp, 4
mov [ebp-88], ebx

mov ebx, [ebp-88]
sub esp, 4
mov [ebp-92], ebx

cmp mov ebx, 0
mov ecx, ebx
mov ebx, [ebp-92]
cmp ebx, ecx
, 0
jne label43
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-96], ebx

mov ebx, [ebp-96]
sub esp, 4
mov [ebp-100], ebx

mov ebx, [ebp-88]
sub esp, 4
mov [ebp-104], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-104]
sub ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-100]
cmp ebx, ecx
, 0
jne label45
label43:
call __exception
label45:
mov ebx, [ebp-88]
sub esp, 4
mov [ebp-108], ebx

mov ebx, [ebp-96]
sub esp, 4
mov [ebp-112], ebx

cmp mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-112]
add ebx, ecx
mov ecx, ebx
mov ebx, [ebp-108]
add ebx, ecx
mov ebx, [ebx]
mov ecx, ebx
mov ebx, [ebp-84]
cmp ebx, ecx
, 0
jne label37
label39:
mov ebx, [ebp-40]
sub esp, 4
mov [ebp-116], ebx

mov ebx, 1
mov ecx, ebx
mov ebx, [ebp-116]
add ebx, ecx
mov [ebp-40], ebx

jmp label32
label37:
mov ebx, 0

mov eax, ebx
mov esp, ebp
mov pop ebp
ret
jmp label39
A_test:
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
mov pop ebp
ret
A_add_int_int:
sub esp, 4
mov ebx, [ebp-4]
sub esp, 4
mov [ebp-8], ebx

sub esp, 4
mov ebx, [ebp-12]
sub esp, 4
mov [ebp-16], ebx

mov ebx, [ebp-8]
sub esp, 4
mov [ebp-20], ebx

mov ebx, [ebp-16]
mov ecx, ebx
mov ebx, [ebp-20]
add ebx, ecx

mov eax, ebx
mov esp, ebp
mov pop ebp
ret
