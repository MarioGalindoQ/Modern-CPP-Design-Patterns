# Visitor Pattern (Acyclic RTTI Version)

```mermaid
classDiagram
   class Visitor {
      <<interface>>
   }

   class Visit~T~ {
      <<interface>>
      +visit(T&)*
   }

   class Accept {
      <<interface>>
      +accept(Visitor&)*
   }

   class As~T~ {
      +accept(Visitor&)
   }

   class Visitable {
      +accept(Visitor&)
   }

   class Red { }
   class Blue { }

   class Triangle {
      +visit(Red&)
      +visit(Blue&)
   }

   class Circle {
      +visit(Red&)
   }

   class Client {
      +main()
   }

   %% Inheritance Hierarchy
   Accept <|.. As~T~
   Visitable <|-- Red
   As~Red~ <|-- Red
   Visitable <|-- Blue
   As~Blue~ <|-- Blue

   Visitor <|.. Triangle
   Visit~Red~ <|.. Triangle
   Visit~Blue~ <|.. Triangle

   Visitor <|.. Circle
   Visit~Red~ <|.. Circle

   %% Composition and Storage
   Client *-- "n" Visitable : colors
   Client *-- "n" Visitor : shapes

   %% Dynamic Relationships (The RTTI jumps)
   Visitable ..> Accept : dynamic_cast
   As~T~ ..> Visit~T~ : dynamic_cast
```

### Design Note:
This diagram illustrates the 'Acyclic Visitor' variation. By using RTTI
(dynamic_cast), we decouple the Visitor interface from the concrete
elements. The base 'Visitor' is now empty, and concrete visitors only inherit
from the 'Visit<T>' interfaces they actually need to implement. This allows
adding new 'Visitable' classes without recompiling the entire visitor hierarchy,
solving the main drawback of the classic GoF pattern.

**Author:** Mario Galindo Queralt, Ph.D.
