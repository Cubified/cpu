;
; every_instruction.S: a program containing every instruction supported by the CPU
;
; Should fail in the simulator, written just to test the assembler.
;

@start:
mov r1, r1
mov r1, 10
add r3, 5
add r4, r3
sub r3, r4
sub r1, 8
mul r2, r1
mul r1, 10
div r2, r1
div r3, 5
cmp r3, r2
cmp r2, 3
je r3
je @done
jne r3
jne @done
jgt r3
jgt @done
jlt r3
jlt @done
jmp r3
jmp @done
end

@done:
ret
