;
; pong.S: atari pong clone
;
; Use W and S to control first paddle,
;   I and K to control second, and Q
;   to quit
;

;
; ENTRY POINT
;
@start:
  ; Input handler
  isr @input

  ; Timer for round start
  mov r2, 0

  ; First paddle position
  mov r1, 7

  ; Second paddle position
  mov r3, 7

  ; Ball position
  mov r14, 8 ; x
  mov r15, 8 ; y

  ; Ball direction
  mov r12, 0 ; x velocity (0 = left, 1 = right)
  mov r13, 0 ; y velocity (0 = up, 1 = none, 2 = down)

  jmp @gameloop

;
; KEYBOARD INPUT
;
@input:
  cmp r0, 119
  jne @padleftdown
  sub r1, 1
  ret
@padleftdown:
  cmp r0, 115
  jne @padrightup
  add r1, 1
@padrightup:
  cmp r0, 105
  jne @padrightdown
  sub r3, 1
@padrightdown:
  cmp r0, 107
  jne @quit
  add r3, 1
@quit:
  cmp r0, 113
  jne @stopisr
  end
@stopisr:
  ret

;
; GAME LOGIC
;
@gameloop:
  cmp r2, 0
  je @continuegame
  slp 0xff
  sub r2, 1
  jmp @gameloop
@continuegame:
  cmp r12, 0
  je @ballleft
  jne @ballright
@calcy:
  cmp r13, 1
  jlt @ballup
  jgt @balldown
@collision:
  jmp @checkleft
@paint:
  jmp @clearstart

;
; BALL MOVEMENT
;
@ballleft:
  sub r14, 1
  jmp @calcy
@ballright:
  add r14, 1
  jmp @calcy

@ballup:
  sub r15, 1
  jmp @collision
@balldown:
  add r15, 1
  jmp @collision

@reset:
  mov r14, 8
  mov r15, 8
  mov r12, 0
  mov r13, 0
  mov r2,  2
  jmp @gameloop

;
; COLLISION DETECTION
;
@checkleft:
  cmp r14, 1
  jgt @checkright
  jlt @reset
  cmp r15, r1
  jgt @checkleftballlower
  jlt @checkleftballhigher
@checkleftballlower:
  mov r11, r15
  sub r11, r1
  cmp r11, 3
  jgt @checkright
  jmp @lefthit
@checkleftballhigher:
  mov r11, r1
  sub r11, r15
  cmp r11, 3
  jgt @checkright
@lefthit:
  mov r12, 1

@checkright:
  cmp r14, 14
  jlt @checktop
  jgt @reset
  cmp r15, r3
  jgt @checkrightballlower
  jlt @checkleftballhigher
@checkrightballlower:
  mov r11, r15
  sub r11, r3
  cmp r11, 3
  jgt @checktop
  jmp @righthit
@checkrightballhigher:
  mov r11, r3
  sub r11, r15
  cmp r11, 3
  jgt @checktop
@righthit:
  mov r12, 0

@checktop:
  cmp r15, 0
  jne @checkbot
  mov r13, 2

@checkbot:
  cmp r15, 15
  jne @paint
  mov r13, 0

;
; RENDER FUNCTIONS
;
@clearstart:
  mov r4, 0    ; Index of current pixel
@clear:
  mov #04, 232 ; Write the clear color (black)
  add r4, 1    ; Move to the next pixel
  cmp r4, 256  ; Check if done (16x16 pixels)
  je @cleardone
  jmp @clear
@cleardone:
  jmp @drawstart

@drawstart:
  mov r4, r1   ; 2d coords to 1d array conversion
  mul r4, 16   ;   index = (y * width) + x,
  add r4, 1    ;   where r4 = y, width = 16, and x = 1
  mov r6, 0

  mov r5, r3   ; Same as above
  mul r5, 16
  add r5, 14
  mov r7, 0
@drawfirst:
  mov #04, 255 ; Draw one pixel of first paddle
  add r4, 16   ; Move down one line (16 pixels)
  add r6, 1
  cmp r6, 3    ; Check if 4 lines have been drawn
  jgt @drawsecond
  jmp @drawfirst
@drawsecond:
  mov #05, 255 ; Same as above
  add r5, 16
  add r7, 1
  cmp r7, 3
  jgt @drawdone
  jmp @drawsecond
@drawdone:
  mov r8, r15  ; Draw ball (same 2d-to-1d transform)
  mul r8, 16
  add r8, r14
  mov #8, 1
  pnt
  slp 0x45
  jmp @gameloop
