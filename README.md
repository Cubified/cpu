## cpu

A simple assembler and simulator for an imaginary 8-bit CPU.

The simulator currently supports 7 distinct instructions (with 16 8-bit general purpose registers), and accepts its own executable format.  The assembler is capable of interpreting an x86-like assembly language and producing binaries which conform to this format.

### Compiling and Running

To compile the simulator:

     $ make
     $ ./cpu
     Usage: cpu [program]

To compile the assembler:

     $ cd assembler
     $ make
     $ ./assembler
     Usage: assembler [file.S]

There is currently only one sample program in `programs/`.  To assemble and run it, execute:

     $ ./assembler/assembler programs/test.S
     $ ./cpu programs/test.S.bin
     Starting execution at entry point 0x05.

     MOV r1 0x0f
     MOV r2 r1
     ADD r2 0x01
     CMP r2 r1
     END

     Registers at end of execution:
       r0 = 00
       r1 = 0f
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

### To-Do

- Error checking (both in the simulator and assembler)
- More instructions
- Labels
