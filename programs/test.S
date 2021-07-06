;
; test.S: a simple test program
;

@start:
  mov r1, 0x0f
@loop:
  sub r1, 1
  cmp r1, 0x00
  je @done
  jmp @loop
@done:
  end
