;
; drawutils.S: drawing utilities
;

#pragma once

;
; r0: clear color
@drawclear:
  mov r15, 255
@drawclearloop:
  add r15, 1
  mov #15, r0
  cmp r15, 255
  jne @drawclearloop
  pnt
  ret
