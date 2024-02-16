# Processor Emulator

This is a simple emulator for a processor written in C. It simulates the execution of machine instructions stored in memory.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [How the Processor Works](#how-the-processor-works)
- [Usage](#usage)
- [File Structure](#file-structure)

## Introduction

This emulator is designed to execute machine instructions stored in memory. It supports basic arithmetic, logical, and data transfer operations.

## Features

- **Instruction Execution**: Executes machine instructions loaded into memory.
- **Arithmetic and Logical Operations**: Supports ADD, SUB, AND, OR, XOR operations.
- **Data Transfer**: Supports LOAD and STORE operations between registers and memory.
- **Error Handling**: Provides error messages for file operations, memory allocation, and invalid instructions.

## How the Processor Works

### Supported Instructions

The processor supports the following instructions:

- **LOAD**: Loads data from memory into registers. Example: `LOAD R1, #4` or `LOAD R1, LOC`.
- **STORE**: Stores data from a register into a memory location. Example: `STORE R1, LOC`.
- **ADD**: Computes the addition of Operand2 and Operand3 and stores the result in Operand1.
- **SUBTRACT**: Computes the subtraction of Operand2 and Operand3 and stores the result in Operand1.
- **AND**: Computes the bitwise AND of Operand2 and Operand3 and stores the result in Operand1.
- **OR**: Computes the bitwise OR of Operand2 and Operand3 and stores the result in Operand1.
- **XOR**: Computes the bitwise XOR of Operand2 and Operand3 and stores the result in Operand1.

### Instruction Encoding

![Encoding](https://github.com/nafis71041/Basic-Processor-in-C/blob/main/encoding.png)

Assembly instructions are converted into machine instructions using a specific encoding scheme:

- The first bit (T-bit) indicates the type of instruction: 1 for data transfer instructions and 0 for arithmetic or logical instructions.
- The next 4 bits represent the instruction opcode, determining the operation to perform.
- For data transfer instructions, the I-bit (Immediate bit) indicates whether Operand2 is a constant value.
- Operand1, Operand2, and Operand3 are encoded as specific bit sequences representing registers or constant values.

### Operand Encoding

- Registers are represented by 3-bit sequences, ranging from R0 to R7.
- Constant values, if used, are represented in 4-bit 2’s complement system.
- For Load and Store instructions, Operand3 is always null (000), and Operand2 is always a memory address.

### Program Data

The program's data is stored in `data.txt`, with each line containing a memory address and the data to be stored at that location. The format of the input files (`program.txt` and `data.txt`) can be understood by referring to the provided sample files.

This system provides a flexible and structured approach to executing machine instructions and handling data within the processor emulator.

## Usage

1. **Clone Repository**: Clone this repository to your local machine.
2. **Compile Program**: Use a C compiler to compile the `processor_emulator.c` file.
    ```bash
    gcc processor_emulator.c -o processor_emulator
    ```
3. **Run Program**: Execute the compiled program.
    ```bash
    ./processor_emulator
    ```
4. **Input Files**: Ensure the input files (`program.txt` and `data.txt`) are present in the same directory as the compiled program.

## File Structure

- `processor_emulator.c`: Main C source code file containing the processor emulator implementation.
- `program.txt`: Text file containing machine instructions.
- `data.txt`: Text file containing data to be loaded into memory.
