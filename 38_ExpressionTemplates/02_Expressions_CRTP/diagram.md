# Expression Templates: CRTP & Static AST

```mermaid
classDiagram
   class VecExpression~Derived~ {
      <<interface>>
      +size() size_t
      +operator[](size_t) double
   }

   class Vector {
      -vector~double~ data_
      +operator=(VecExpression)
   }

   class VecSum~L, R~ {
      -L& lhs_
      -R& rhs_
      +operator[](size_t) double
   }

   class VecScale~E~ {
      -double scalar_
      -E& expr_
      +operator[](size_t) double
   }

   class Client {
      +main()
   }

   %% Static Inheritance (CRTP)
   VecExpression~Vector~ <|-- Vector
   VecExpression~VecSum~ <|-- VecSum~L, R~
   VecExpression~VecScale~ <|-- VecScale~E~

   %% Composition of the Static AST (References to operands)
   VecSum *-- "1" L : lhs_
   VecSum *-- "1" R : rhs_
   VecScale *-- "1" E : expr_

   %% The Vector assignment triggers the loop fusion
   Vector ..> VecExpression : evaluates

   Client *-- "n" Vector
   Client ..> VecSum : via operator+
   Client ..> VecScale : via operator*
```

### Design Note:
This diagram shows the classic CRTP-based Expression Template architecture. 
The operators no longer return data; they return 'Proxy Objects' (VecSum, VecScale) 
that form a static Abstract Syntax Tree (AST). All nodes inherit from 'VecExpression', 
allowing the 'Vector' class to accept any complex tree in its assignment operator, 
triggering a single fused loop that eliminates temporary allocations.

**Author:** Mario Galindo Queralt, Ph.D.
