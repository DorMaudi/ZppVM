# ZppVM

ZppVM is a small educational **stack-based virtual machine** written in C++.  
It executes a custom binary bytecode format stored in `.zpp` files.

The project focuses on demonstrating core VM concepts such as instruction decoding,
stack-based execution, binary file loading, and runtime validation.  
It is intentionally minimal and designed for learning and demonstration purposes.

---

## Features

- Custom binary executable format (`.zpp`)
- Minimal VM loader with strict header validation
- Stack-based execution model
- Signed 64-bit integer arithmetic
- Explicit runtime error handling

---

## Virtual Machine Architecture

### Execution Model

ZppVM uses a **stack-based architecture**:

- Instructions operate on a value stack
- Arithmetic instructions pop operands and push results
- An instruction pointer (`ip`) processes bytecode sequentially
- Execution stops on `HALT` or on runtime error

---

## Supported Opcodes

| Opcode (hex) | Name | Stack Effect | Description |
|--------------|-----|--------------|-------------|
| `0x00` | SKIP (NOP) | — | No operation |
| `0x01` | PUSH_I64 | — → value | Push a signed 64-bit integer |
| `0x02` | ADD | a, b → a + b | Addition |
| `0x03` | SUB | a, b → a − b | Subtraction |
| `0x04` | HALT | — | Stop execution |
| `0x05` | MUL | a, b → a × b | Multiplication |
| `0x06` | DIV | a, b → a ÷ b | Division (error on divide-by-zero) |
| `0x07` | MOD | a, b → a % b | Modulo |
| `0x08` | POP | a → — | Pop value from stack |
| `0x09` | SWAP | a, b → b, a | Swap top two stack values |

---

## `.zpp` File Format (Version 1)

A `.zpp` file consists of:
[8-byte header][raw bytecode...]

### Header Layout

| Offset | Size | Meaning |
|------|------|---------|
| 0x00 | 4 | Magic number: `7A 70 70 00` (`"zpp\0"`) |
| 0x04 | 2 | Version (little-endian), must be `0x0001` |
| 0x06 | 2 | Flags (must be `0x0000` in v1) |

Files with invalid headers are rejected by the loader.

---

## Bytecode Encoding

- Instructions use **1-byte opcodes**
- `PUSH_I64` is followed by an **8-byte signed integer (little-endian)**
- All other instructions are single-byte

### Example Program
PUSH_I64 10
PUSH_I64 3
SUB
HALT


Encoded bytecode:

01 0A 00 00 00 00 00 00 00
01 03 00 00 00 00 00 00 00
03
04

---

## Example `.zpp` File (Hex)

7A 70 70 00 01 00 00 00
01 0A 00 00 00 00 00 00 00
01 03 00 00 00 00 00 00 00
03
04


This program evaluates `10 - 3` and halts with `7` left on the stack.
