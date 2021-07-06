## cpu

A simple assembler and simulator for an imaginary 8-bit CPU.

The simulator currently supports 13 distinct instructions (with 16 8-bit general purpose registers), and accepts its own executable format.  The assembler is capable of interpreting an x86-like assembly language and producing binaries which conform to this format.

### Compiling and Running

To compile the simulator:

```shell
     $ make
     $ ./cpu
     Usage: cpu [program]
```

To compile the assembler:

```shell
     $ cd assembler
     $ make
     $ ./assembler
     Usage: assembler [file.S]
```

There is currently only one sample program in `programs/`.  To assemble and run it, execute:

```shell
     $ ./assembler/assembler programs/test.S
     $ ./cpu programs/test.S.bin
     Starting execution at entry point 0x05.

     mov r1, 0x0f
     sub r1, 1
     cmp r1, 0x00
     je 0x12
     jmp 0x08
     ...
     end

     Registers at end of execution:
       r0 = 00
       r1 = 00
       r2 = 10
       r3 = 00
       r4 = 00
       r5 = 00
       r6 = 00
       r7 = 00
       r8 = 00
       r9 = 00
       r10 = 00
       r11 = 00
       r12 = 00
       r13 = 00
       r14 = 00
       r15 = 00
     Executed with return value 0.
```

### Assembly Language

The assembly language understood by the assembler is a mix of x86- and 6502-inspired assembly.  Here is a sample program which demonstrates its syntax:

```asm
;
; Comments are denoted by semicolons
;

@start:        ; Labels are denoted by at signs (execution will always begin at the @start label, or 0x05 if not found)
  mov r1, 0x0f ; Instructions can be uppercase or lowercase, and registers are denoted as r1, r2, r3, etc. (up to r15)
@loop:
  sub r1, 1    ; Numeric/immediate-mode operands can be in either hex or decimal
  cmp r1, 0x00
  je @done     ; Indentation and spacing are flexible
  jmp @loop
@done:
  end          ; Execution must end with an "end" instruction, otherwise the simulator will run infinitely
```

### To-Do

- Better error checking (both in the simulator and assembler)
- More programs
