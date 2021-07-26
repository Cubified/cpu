## cpu

A simple assembler and simulator for an imaginary 8-bit CPU.

The simulator currently supports 17 distinct instructions (with 16 8-bit general purpose registers), and accepts its own executable format.  The assembler is capable of interpreting an x86-like assembly language and producing binaries which conform to this format.

Additionally, 256-color graphics can be drawn using direct memory manipulation of the CPU's accessible VRAM, and user input can be read using an interrupt handler (more information on both of these below).

### Demo

![demo.gif](https://github.com/Cubified/cpu/blob/main/demo.gif)

### Compiling and Running

To compile the simulator:

```sh
$ make
$ ./cpu
Usage: cpu [program]
```

To compile the assembler:

```sh
$ cd assembler
$ make
$ ./assembler
Usage: assembler [file.S]
```

There are a few example programs in `programs/`, including a [clone of Atari Pong](https://github.com/Cubified/cpu/blob/main/programs/pong.S).  To assemble and run one in debug mode (w/ disassembly), execute:

```sh
$ ./assembler/assembler programs/test.S
$ ./cpu programs/test.S.bin --debug
Starting execution at entry point 0x000a.

mov r0, 0x0f
sub r0, 1
cmp r0 (16), 0x00
jne 0x000d
...
end

Registers at end of execution:
  r0 = 00
  r1 = 00
  r2 = 10
  ...
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
  mov #01, 0   ; Pound sign denotes VRAM access, with this instruction writing 0 to the VRAM address pointed to by register 1
  cmp r1, 0x00
  je @done     ; Indentation and spacing are flexible
  jmp @loop
@done:
  end          ; Execution must end with an "end" instruction, otherwise the simulator will run infinitely
```

### Graphics and User Input

The imaginary CPU has access to 256 bytes of video memory, which can be written to freely using the `mov` instruction and displayed onscreen using the `pnt` instruction.  The syntax for these instructions is as follows:

```asm
mov r0, 128  ; Register 0 now stores the index of the pixel we wish to modify (128)
mov #00, 255 ; This mov instruction writes a pixel value of 255 to the VRAM address pointed to be register 0 (i.e. pixel 128)
pnt          ; Render the changes to VRAM onscreen (uses ANSI escape codes under the hood)
```

User input is processed using an interrupt service routine (ISR).  To set a label as the ISR, use the `isr` instruction.  For example:

```asm
@start:
  isr @inputhandler ; Set the label "@inputhandler" as the interrupt service routine
@loop:
  ; If input is received, code running here will be interrupted (then resumed with the "ret" instruction)
  jmp @loop

@inputhandler:
  cmp r0, 97   ; The CPU simulator fills register 0 with the char value of the key pressed on the keyboard (lowercase a = 97)
  je @aispressed
  ret          ; The link register is set to where the CPU was interrupted, meaning the ret instruction will return execution to normal
@aispressed:
  mov r1, 0
  mov #01, 255
  pnt          ; Draw a white pixel every time the a key is pressed
  ret
  
```

### [Documentation](https://github.com/Cubified/cpu/blob/main/doc)

Documentation covers each instruction, accessible memory, CPU layout, binary format, and the assembly language in greater detail.

### To-Do

- Better error checking (both in the simulator and assembler)
- Better collision physics in `pong.S`
- More comments in sample programs
