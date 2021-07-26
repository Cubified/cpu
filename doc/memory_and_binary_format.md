## CPU Layout and Binary Reference

### Registers and Memory

| Name                      | How to Access                            | Size                    | Description                                                                                                                              |
|---------------------------|------------------------------------------|-------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| General-Purpose Registers | `r0, r1, r2, ... r15`                    | 1 byte (8 bits)         | 16 registers which can be used freely.  If an interrupt is fired due to a keypress, register 0 is modified to store the value of the key |
| Link Register             | `ret` (modify w/ `call`)                 | 2 bytes (16 bits)       | Stores the index of the most recently-added address on the call stack.  `ret` returns execution to this address                          |
| Flag Register             | `je, jne, jgt, jlt` (modify w/ `cmp`)    | 1 byte (8 bits)         | Stores the result of the most recent comparison, with four possible values:  None, equal, greater, or less                               |
| VRAM                      | Cannot be read, modify w/ `mov #00, ...` | 256 bytes (1 per pixel) | Stores the raw pixel data of video memory                                                                                                |

### Binary Format

| Offset | Name        | Description                                                            |
|--------|-------------|------------------------------------------------------------------------|
| `0x00` | Text Start  | Address of the start of the text (code) section                        |
| `0x10` | Text Length | Length of the binary's text section                                    |
| `0x20` | Data Start  | Address of the start of the data section                               |
| `0x30` | Data Length | Length of the binary's data section                                    |
| `0x40` | Entry Point | Program entry point (default is `0x0a` if `@start` label is not found) |
