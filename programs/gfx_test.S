;
; gfx_test.S: simple graphics/VRAM test
;

@start:
  mov r1, 0
@restart:
  mov r0, 0
  add r1, 1
  pnt
  slp 0xff
@loop:
  add r0, 1
  mov #00, r1
  cmp r0, 256
  je @restart
  jmp @loop
