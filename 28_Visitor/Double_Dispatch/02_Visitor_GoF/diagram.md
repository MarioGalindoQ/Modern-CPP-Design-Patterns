# Visitor Pattern (GoF Double Dispatch)

```mermaid
classDiagram
    class Visitable {
        <<interface>>
        +accept(Visitor&)*
    }
    class Red {
        +accept(Visitor&)
    }
    class Blue {
        +accept(Visitor&)
    }

    class Visitor {
        <<interface>>
        +visit(Red&)*
        +visit(Blue&)*
    }
    class Triangle {
        +visit(Red&)
        +visit(Blue&)
    }
    class Circle {
        +visit(Red&)
        +visit(Blue&)
    }

    class Client {
        +main()
    }

    class dispatch_error {
        <<exception>>
    }

    %% Inheritance (Is_a)
    Visitable <|.. Red
    Visitable <|.. Blue
    Visitor <|.. Triangle
    Visitor <|.. Circle

    %% Composition (Has_a)
    Client *-- "n" Visitable : colors
    Client *-- "n" Visitor : shapes

    %% The Handshake (Double Dispatch logic)
    Red ..> Visitor : calls visit()
    Blue ..> Visitor : calls visit()
    
    %% Dependency
    Circle ..> dispatch_error : throws
```

### Design Note:
In this classic GoF implementation, we decouple the hierarchy of elements
(Colors) from the hierarchy of operations (Shapes). The 'Visitable' interface
defines the 'accept' method, which acts as the entry point for the
handshake. The 'Visitor' interface must declare a 'visit' method for every
concrete type in the Visitable hierarchy. This creates a cyclic dependency but
provides a type-safe way to resolve both the element and the operation types at
runtime without manual type checking.
