;
; message.S: prints a message
;

@start:
  mov r0, 17
  mov #00, 255
  add r0, 6
  mov #00, 255
  add r0, 10
  mov #00, 255
  add r0, 4
  mov #00, 255
  add r0, 2
  mov #00, 255
  add r0, 10
  mov #00, 255
  add r0, 1
  mov #00, 255
  add r0, 5
  mov #00, 255
  add r0, 10
  mov #00, 255
  add r0, 2
  mov #00, 255
  add r0, 2
  mov #00, 255
  add r0, 12
  mov #00, 255
  add r0, 2
  mov #00, 255
  add r0, 2
  mov #00, 255
  add r0, 2
  mov #00, 255
  pnt
  slp 0xffff
  end
