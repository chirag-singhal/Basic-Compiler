# Basic Compiler

This is a basic example of a compiler for a language as described in the problem statement. It designs the grammar correctly incorporating all features and constructs defined in
the language specification. The grammar is unambiguous in such a way that it produces only one parse tree for the given input source code. It implements - 

    *   Grammar and grammar parser
    *   Tokeniser
    *   Lexer
    *   Type expression table generator
    *   Type errors generator
    *   Parse Tree Generator



**To compile the compiler :**
```
    make compile
```

**To run separate testcases :**
```
    make run_t1
```
Similarly, run_t2, run_t3, ..., run_t6


**After completion of testing :**
```
    make end
```
