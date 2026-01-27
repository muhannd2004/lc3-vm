# LC3-VM

LC-3 (Little Computer 3) virtual machine written in C.

## What is LC-3?

LC-3 is a 16-bit educational computer architecture. This VM emulates its CPU by implementing:

- **16-bit registers** (R0-R7, PC, COND)
- **65,536 memory locations**
- **15 opcodes**: ADD, AND, NOT, BR, JMP, JSR, LD, LDI, LDR, LEA, ST, STI, STR, TRAP
- **Condition flags**: Negative, Zero, Positive
- **Trap routines**: GETC, OUT, PUTS, IN, PUTSP, HALT

## How It Works

1. Load a `.obj` binary into memory at address `0x3000` this's the deafult you can change it by changing the first 16 bit of the `.obj`.
2. Fetch-decode-execute loop reads instructions, decodes opcodes, and executes operations
3. Memory-mapped I/O handles keyboard input via `MR_KBSR`(status) and `MR_KBDR` (value)

**Full ISA documentation**: [docs/lc3-isa.pdf](docs/lc3-isa.pdf)

### 

## Build & Run

```bash
//Step 1: compile .c → .o
make clean

make
```

### How To Run

```bash
./bin/lc3 {$} #the $ could be path to any image you have check img dir

```