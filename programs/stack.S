;
; stack.S: call stack test
;

@start:
  mov r0, 0
@loop:
  add r0, 1
  cmp r0, 255
  je @undo
  call @loop
@undo:
  sub r0, 1
  cmp r0, 0
  je @done
  ret
@done:
  end
