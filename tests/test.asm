
stupid line
  mov eax, ebx
  test eax
  jz loc_0
  call sub_0
loc_0:
  mov eax, ebx
  retn

sub_0 proc near
  mov eax, ebx
loc_1:
  test eax
  jz loc_2
  call sub_1
  jmp loc_1
loc_2:
  mov eax, ebx
  retn
sub_0 endp

  mov eax, ebx
  retn

  mov eax, ebx
  retn

sub_1 proc near
  mov eax, ebx
  test eax
  jz loc_3
  call sub_2
loc_3:
  retn
sub_1 endp

  mov eax, ebx
loc_4:
  add eax, 5
  test eax
  jnz loc_4
  test eax

sub_2 proc near
  mov eax, ebx
  retn
sub_2 endp

