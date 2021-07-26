;
; test.S: an extremely simple test program
;

@start:
  mov r0, 16
@loop:
  sub r0, 1
  cmp r0, 0
  jne @loop
  end
