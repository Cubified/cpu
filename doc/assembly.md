## Assembly Language Reference

### Basic Overview

The assembly language understood by the assembler is inspired by my favorite parts of both x86 and 6502 assembly.  Although the syntax is heavily inspired by both of these, it is still a very minimal assembly language, consisting of some nice-to-haves but primarily being focused on being rock-solid in the features it does provide.

This document describes in more detail the properties of the assembly language, as well as provides a list of the features it lacks when compared with a more feature-rich assembly language.

### Labels

Labels are human-readable names which refer to a 16-bit address in a given program.  When a source file is assembled, two "passes" occur:

 1. Label definitions are located and added to a list of labels.  At the same time, an instruction pointer is incremented with every valid instruction, and every new label's address is calculated using this.
 2. The program is assembled from top-to-bottom.  When a label reference is encountered (e.g. a jump instruction), its corresponding list entry is located, and the label name is replaced with its precomputed address.

#### Syntax

All label definitions and references *must* begin with an at sign (@).  Outside of this, labels have no restrictions in their naming other than having 64 characters allotted to each.

```asm
@start:      ; This is a label definition
  jmp @start ; This is a label reference
```

### Registers

Registers are 1-byte pieces of memory capable of storing data in the range [0, 255].  The CPU has access to 16 general-purpose registers.

#### ISR

When using an interrupt service routine, register 0 is modified to contain the value of the key most recently pressed.  As a result, using register 0 to store any meaningful value while also using an ISR is not recommended.

#### Syntax

All register references must begin with the letter `r`, followed the number of the register to access.

```asm
  mov r1, 0  ; Moves 0 into register 1
  mov r01, 0 ; Does the same thing -- either naming convention works
```

### Rendering Pipeline

At the assembly level, VRAM can be modified using the `mov` instruction, and rendered using the `pnt` instruction.  A sample use-case is as follows:

```asm
@start:
  mov r0, 128  ; Pixel to be accessed
  mov #00, 255 ; Write a white pixel (color 255) to the VRAM address pointed to by register 0 (i.e. 128)
  pnt          ; Render the new VRAM onscreen
```

Under the hood of the simulator, this assembly translates into:

1. Write 128 into `regs[0]`
2. Write 255 into `vram[regs[0]]`
3. Loop over each pixel in `vram`, set the pixel position using `"\x1b[{y};{x}H"` and pixel color using `"\x1b[{color}m"`, then printf the string to the terminal

### What features are not (yet) included

In no particular order:

- Sections (e.g. data, text, bss, etc.) -- the binary format makes room for this, but neither the assembler nor the simulator make meaningful use of it
- Constants
- Includes
- Memory accesses (including relative access) -- also, non-VRAM memory in general
