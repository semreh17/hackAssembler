# Hack Assembler (Nand2Tetris)

A custom assembler written in C for the Hack computer architecture, developed as part of a Computer Architecture class based on the *Nand2Tetris* project.

This program translates symbolic Hack Assembly Language (`.asm`) into 16-bit binary machine code (`.hack`) that can be executed directly by the Hack CPU.

## structure
* `main.c`: handles file I/O and program execution flow.
* `assembler.c`: contains the core logic for the two-pass assembly process.
* `parser.c`: identifying instruction types, translating mnemonic components (comp, dest, jump) into binary values.
* `symbolTable.c`: implements the dynamic hash table used to manage memory addresses for variables and jump labels.

### Compilation

```bash
make
