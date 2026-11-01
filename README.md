# CCompiler

CCompiler is a small C compiler written in C++. It tokenizes C source, builds an abstract syntax tree, and generates AArch64 assembly.

## Features

- Lexing and parsing of core C syntax
- Functions, parameters, local and global declarations
- Arithmetic, comparisons, logical and bitwise expressions
- Conditionals and loops (`if`, `while`, and `for`)
- Pointers, arrays, structs, function calls, and recursion
- Optional token and AST output for debugging

The examples in [`src/tests`](./src/tests) provide progressively larger language-coverage cases, ending with [`t20_capstone.c`](./src/tests/t20_capstone.c).

## Requirements

- A C++ compiler with C++17 support
- An AArch64 assembler/toolchain if you want to assemble and run the generated output

On Windows, the included VS Code task expects g++ at `C:\msys64\ucrt64\bin\g++.exe`.

## Build

From the repository root:

```powershell
g++ -std=c++17 src\lexer\lexer.cpp src\parser\parser.cpp src\generator\generator.cpp src\main.cpp -o ccompiler.exe
```

Alternatively, run the default build task in VS Code.

## Usage

```powershell
.\ccompiler.exe <input-file> <output-file>
```

For example:

```powershell
.\ccompiler.exe src\tests\t01_return_const.c output.s
```

The compiler prints tokens and the parsed AST by default. Use `--help` to display the command syntax. The `--debug` option is also accepted.

The generated `output.s` contains AArch64 assembly and must be assembled with a suitable AArch64 toolchain before it can be executed.

## Project layout

```text
src/
  lexer/       Lexer and token definitions
  parser/      AST definitions and parser
  generator/   AArch64 assembly generator
  tests/       C language feature examples
  main.cpp     Command-line entry point
```
