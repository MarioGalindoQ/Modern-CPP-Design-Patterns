============================================================================
                        VISITOR PATTERN (BEHAVIORAL)
============================================================================

Intent:
The Visitor pattern represents an operation to be performed on the elements 
of an object structure. It lets you define a new operation without changing 
the classes of the elements on which it operates.

The Core Problem:
In C++, virtual functions only provide "Single Dispatch". This makes it 
hard to perform operations that depend on the runtime types of TWO objects 
(Double Dispatch) or to add new operations to a stable hierarchy without 
violating the Open/Closed Principle.

Our Approach:
We have divided the study of this pattern into two logical domains to cover 
both its mechanical and architectural facets:

1. Double_Dispatch/:
   Focuses on the "handshake" mechanism. These examples solve the problem of 
   resolving types for two interacting objects (e.g., shape collisions).
   - Visitor_Simple/: Basic recursive virtual call approach.
   - Visitor_GoF/: The classic Gang of Four implementation.
   - Visitor_RTTI/: Acyclic Visitor using dynamic_cast to break dependencies.
   - Visitor_Modern_Variant/: C++17 'std::variant' and 'std::visit' approach.

2. Extending_Behaviors/:
   Focuses on the architectural power of the pattern. These examples use 
   Visitor to add new operations (Export, Text Extraction) to a stable 
   Document structure, demonstrating how to keep object classes clean and 
   focused while centralizing algorithmic logic.
   - Visitor_Traditional_GoF/: Classic approach using accept() methods.
   - Visitor_Modern/: Non-intrusive approach using 'std::variant'.

----------------------------------------------------------------------------
SEE ALSO: STATIC VISITOR (Pattern 15_Mixin)
For a compile-time version of this pattern, refer to the 'Advanced Mixin' 
example. It demonstrates how to iterate over components with zero runtime 
overhead using Variadic Templates and Fold Expressions (C++17).
============================================================================
