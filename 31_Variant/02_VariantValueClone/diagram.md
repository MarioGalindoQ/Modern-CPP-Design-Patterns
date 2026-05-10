std::variant (Modern C++17 Feature) - Value Semantics Clone

```mermaid
classDiagram
   class Shape {
      <<abstract>>
      +draw()* void
   }

   class Square {
      +Square()
      +Square(Square& other)
      +Square(Square&& other)
      +draw() void
   }

   class Circle {
      +Circle()
      +Circle(Circle& other)
      +Circle(Circle&& other)
      +draw() void
   }

   class ShapeVariant {
      <<alias>>
      std::variantSquare, Circle
   }

   class Client {
      +main()
      -vectorShapeVariant originals
      -vectorShapeVariant clones
   }

   %% Relationships
   Square --|> Shape : Square is a Shape
   Circle --|> Shape : Circle is a Shape

   ShapeVariant ..> Square : uses
   ShapeVariant ..> Circle : uses

   %% The Client owns the collection of variants by value
   Client *-- "n" ShapeVariant : owns

   %% Interaction via std::visit
   Client ..> ShapeVariant : std::visit

```

### Design Note:
This diagram illustrates the Modern Prototype Pattern using Value Semantics.
Unlike the traditional approach (see 32_CRTP/03_PolymorphicClone), we do not use
heap-allocated pointers or a virtual 'clone()' method.

    1. Inheritance for Interface: Square and Circle inherit from Shape
       to maintain a consistent contract, but polymorphism is resolved statically.

    2. Value Semantics: The ShapeVariant alias represents a type-safe union
       that stores the objects directly (usually on the stack or in-place within
       the container).

    3. Automatic Deep Copy: Cloning is achieved through the standard assignment
       operator (clones = originals). The std::variant infrastructure
       automatically dispatches the appropriate Copy or Move constructors for the
       contained types.

    4. Optimization: The use of noexcept in move constructors is crucial here,
       as it allows std::vector to move elements efficiently during reallocation
       without falling back to expensive copies.

**Author:** Mario Galindo Queralt, Ph.D.
