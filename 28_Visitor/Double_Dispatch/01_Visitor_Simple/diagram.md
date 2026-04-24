# Visitor Pattern (Simple Double Dispatch)

```mermaid
classDiagram
   class Visitable_Visitor {
      <<interface>>
      +intersect(Visitable_Visitor&)*
      +intersect(Circle&)*
      +intersect(Triangle&)*
   }

   class Circle {
      +intersect(Visitable_Visitor& other)
      +intersect(Circle&)
      +intersect(Triangle&)
   }

   class Triangle {
      +intersect(Visitable_Visitor& other)
      +intersect(Circle&)
      +intersect(Triangle&)
   }

   class Client {
      +main()
   }

   %% Inheritance (Is_a) - No numbers used
   Visitable_Visitor <|.. Circle
   Visitable_Visitor <|.. Triangle

   %% The Client manages a collection of these dual-role objects
   %% Composition (Has_a) - Multiplicity "n" at the end
   Client *-- "n" Visitable_Visitor : shapes

   %% Handshake mechanism: objects interact through their base interface
   %% Dependency (..>) - No numbers
   Visitable_Visitor ..> Visitable_Visitor : handshake dispatch
```

### Design Note:
In this "Simple" version, the 'Double Dispatch' problem is solved by making
every object both the receiver and the executor of the interaction. The
'Visitable_Visitor' interface forces every concrete shape to handle every other
shape type. When 'intersect' is called on a base pointer, the object uses its
own concrete identity (*this) to trigger the correct overload in the second
object, resolving the types of both participants at runtime.
