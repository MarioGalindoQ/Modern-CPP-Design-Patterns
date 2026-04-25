# Decorator Pattern (Static / Mixin Inheritance)

```mermaid
classDiagram
   class Coffee_ToBeDecorated {
      +getDescription() string
      +getCost() double
   }

   class Milk~Decorated~ {
      +getDescription() string
      +getCost() double
   }

   class Sugar~Decorated~ {
      +getDescription() string
      +getCost() double
   }

   class Vanilla~Decorated~ {
      +getDescription() string
      +getCost() double
   }

   class Client {
      +main()
   }

   %% In Static Decorator, the decorator inherits from the template parameter Decorated.
   %% There is NO composition (*--) and NO multiplicity numbers.
   
   Decorated <|-- Milk~Decorated~
   Decorated <|-- Sugar~Decorated~
   Decorated <|-- Vanilla~Decorated~

%% Client instantiates concrete types
   Client ..> Decorated
```

### Design Note:
In this version, the 'is a' relationship is established at compile-time. There
is no 'has a' relationship because the decorator does not hold a pointer to
another object; it literally inherits the functionality of the class passed as a
template parameter.

**Author:** Mario Galindo Queralt, Ph.D.
