# 🛠️ Zar - A Custom Compiler Project (C++)

A **lightweight, educational compiler** implemented in C++, built to explore core compiler construction concepts such as lexing, parsing, AST design, and early-stage code generation.

---

## ✨ Implemented Features

### Lexical Analysis
- Tokenization of identifiers, keywords, literals, operators, and delimiters
- Basic error reporting for invalid tokens

### Syntax Analysis
- Handwritten parser (recursive descent)
- Constructs a well-structured **Abstract Syntax Tree (AST)**
- Supports a limited subset of language constructs

### Abstract Syntax Tree (AST)
- Clear node hierarchy for expressions, statements, and declarations
- Visitor-based traversal model
- Designed to support future compiler passes

### Code Generation (Partial / Experimental)
- Early-stage backend scaffolding
- Architecture prepared for LLVM IR emission
- No guarantees of correctness or completeness

---

## 🚫 What This Compiler Cannot Do (Limitations)

This compiler is **not feature-complete** and **not intended for real-world usage**.

### Language & Semantics
- ❌ No semantic analysis
- ❌ No type checking
- ❌ No symbol resolution validation
- ❌ No enforcement of scope rules
- ❌ No meaningful compile-time diagnostics

### Code Generation
- ❌ Cannot generate fully working executables
- ❌ No verified LLVM IR output
- ❌ No optimization passes
- ❌ No register allocation or target-specific lowering

### Tooling & Usability
- ❌ No standard library
- ❌ No runtime system
- ❌ No debugger or tooling support
- ❌ No stable language specification
- ❌ No backward compatibility guarantees

### Stability
- ❌ Assumes syntactically valid input in many cases
- ❌ Undefined behavior for malformed or edge-case programs
- ❌ Frequent internal refactors expected

> In short: this is a **toy / learning compiler**, not a production compiler and not a scripting language runtime.



## 🧱 Compiler Pipeline (Current)

```text
Source Code
    ↓
Lexer
    ↓
Parser
    ↓
Abstract Syntax Tree (AST)
    ↓
(Codegen scaffolding — incomplete)
```

## ⚙️ Build Instructions

### Requirements
- C++17 or later  
- `cmake`  
- `clang` or `g++`  
- (Optional) LLVM development libraries  

### Build Steps
```bash
mkdir build
cd build
cmake ..
make

## ▶️ Usage

### Running the Compiler
```bash
./zarc <source_file>

### Example
./zarc test/file.zar
