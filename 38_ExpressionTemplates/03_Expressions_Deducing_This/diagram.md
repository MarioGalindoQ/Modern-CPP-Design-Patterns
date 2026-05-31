# Expression Templates: Deducing This (C++23)

```mermaid
classDiagram
   class VecExpression {
      +size(this auto&& self)
      +operator[](this auto&& self, size_t)
   }

   class Vector {
      -vector~double~ data_
      +operator=(Expression)
   }

   class VecSum~L, R~ {
      -L& lhs_
      -R& rhs_
      +size()
      +operator[](size_t)
   }

   class VecScale~E~ {
      -double scalar_
      -E& expr_
      +size()
      +operator[](size_t)
   }

   class Client {
      +main()
   }

   %% Standard Inheritance (No longer a template base)
   VecExpression <|-- Vector
   VecExpression <|-- VecSum~L, R~
   VecExpression <|-- VecScale~E~

   %% Composition of the AST
   VecSum *-- "1" L
   VecSum *-- "1" R
   VecScale *-- "1" E

   %% Client interaction
   Client *-- "n" Vector
   Client ..> VecExpression : builds AST
```

### Design Note:
This diagram represents the modern C++23 evolution using "Deducing This". 
The 'VecExpression' base class is no longer a template, which simplifies 
the inheritance hierarchy significantly. Using explicit object parameters, 
the base class automatically deduces the concrete node type (Vector, VecSum, 
etc.) at the call site. This provides the same high-performance Loop Fusion 
as the CRTP version but with much cleaner and more maintainable code.

**Author:** Mario Galindo Queralt, Ph.D.
