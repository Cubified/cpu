;
; includes.S: tests the assembler's preprocessor
;

#include drawutils.S

@start:
  mov r0, 128
  call @drawclear
  slp 0xffff
  end
