# CRTP: Polymorphic Clone (Smart Mixin)

```mermaid
classDiagram
   class Shape {
      <<interface>>
      +draw()*
      +~Shape()
   }

   class ICloneable {
      <<interface>>
      +clone_internal()* ICloneable*
   }

   class Cloneable~T~ {
      -friend T
      -Cloneable()
      +clone_internal() ICloneable*
   }

   class Square {
      +Square()
      +Square(const Square&)
      +draw()
   }

   class Circle {
      +Circle()
      +Circle(const Circle&)
      +draw()
   }

   class Triangle {
      +Triangle()
      +draw()
   }

   class Client {
      +main()
      +Cloneable_clone(Base*)
   }

   %% Inheritance (Is_a)
   Shape <|.. Square
   Shape <|.. Circle
   Shape <|.. Triangle
   
   ICloneable <|.. Cloneable~T~
   
   %% Multiple Inheritance (The Mixin injection)
   Cloneable~Square~ <|.. Square
   Cloneable~Circle~ <|.. Circle

   %% Dependencies (Creates / Cross-Casts)
   Cloneable~T~ ..> T : instantiates
   Client ..> ICloneable : cross-casts to
   Client ..> Shape : manages
```

### Design Note:
This diagram illustrates the "Non-Intrusive Prototype" architecture. The 'Shape'
interface remains pure, containing only business logic. Cloning is injected
"from the side" using Multiple Inheritance.
1. Automation: The 'Cloneable' Mixin uses CRTP to implement the copy logic once
for all classes.
2. Safety: The private constructor and 'friend T' in the Mixin ensure only the
correct class can inherit from its specialized template.
3. Decoupling: The 'Client' uses the 'Cloneable_clone' utility to perform a
runtime "Cross-Cast" (dynamic_cast) from the 'Shape' branch to the 'ICloneable'
branch. If an object (like Triangle) does not inherit from the Mixin, the system
detects it and throws an exception, preserving architectural integrity.

**Author:** Mario Galindo Queralt, Ph.D.
