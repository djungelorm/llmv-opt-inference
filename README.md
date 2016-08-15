# llvm-opt-inference
Optimize a subset of LLVM IR using inference

## Accepted LLVM IR

Accepts a subset of LLVM IR:
 * Opcodes are limited to arithmetic and binary opcodes
 * All registers must be of type int32
 * Register names are limited to the set: %0 .. %10
