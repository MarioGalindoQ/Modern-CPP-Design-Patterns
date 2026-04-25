# Interpreter Pattern (Stack Machine Version)

```mermaid
classDiagram
    class OpCode {
        <<enumeration>>
        PUSH
        ADD
        SIN
        ...
    }

    class Instruction {
        <<struct>>
        +OpCode code
        +double value
    }

    class Lexer {
        -string input_
        -size_t pos_
        +nextToken()
        +getToken() Token
    }

    class Compiler {
        -Lexer lexer_
        -vector~Instruction~ program_
        +compile() vector~Instruction~
    }

    class VirtualMachine {
        +evaluate(vector~Instruction~) double
        +printProgram(vector~Instruction~)
    }

    class Client {
        +main()
    }

    Compiler *-- Lexer
    Compiler *-- "n" Instruction
    
    VirtualMachine ..> Instruction
    
    Client ..> Compiler
    Client ..> VirtualMachine
```

### Design Note:
In the Stack Machine version, the architecture is linear. The Compiler performs
a recursive descent to translate the input string into a flat sequence of
Instruction objects (Bytecode). This bytecode is stored in a vector and then
passed to the VirtualMachine, which executes it using an internal stack. Unlike
the AST version, this design prioritizes execution speed and memory efficiency
over a tree-like object structure.

**Author:** Mario Galindo Queralt, Ph.D.
