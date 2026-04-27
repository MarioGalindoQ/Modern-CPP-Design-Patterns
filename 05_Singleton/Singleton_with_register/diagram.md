# Singleton Pattern (with Encapsulated Registry)

```mermaid
classDiagram
   class Singleton {
      -static singletonMap$
      #getSingleton(string_view)$ Singleton*
      +Singleton(string)
      +~Singleton()
   }

   class Service {
      +getService(string_view)$ Service*
      +method_1()*
      +method_2()*
   }

   class Singleton_A {
      -static registered_$ bool
      -Singleton_A(string)
      +method_1()
   }

   class Singleton_B {
      -static registered_$ bool
      -Singleton_B(string)
      +method_2()
   }

   %% Hierarchy
   Singleton <|-- Service : Is_a
   Service <|-- Singleton_A : Is_a
   Service <|-- Singleton_B : Is_a

   %% The Registry Relationship
   Singleton *-- "n" Singleton : 1 singletonMap (Static Data Segment)

   %% Documentation Notes
   note for Singleton_A "Self-registers via \nstatic lambda"
   note for Singleton "Instances persist in \nStatic Data Segment"
```

### Key Architectural Features:
1. **Encapsulated Registration**: Each concrete class (`Singleton_A`,
`Singleton_B`) owns its registration logic through the private `static bool
registered_` member.
2. **Visibility Control**: Constructors of concrete classes are `private`,
ensuring that instances can only be created by the internal auto-registration
mechanism.
3. **Static Storage**: All registered instances live in the **Static Data
Segment**, providing high performance and avoiding heap fragmentation.
4. **Type-Safe Access**: The `Service` class provides a specialized access point
(`getService`) that returns the correct interface type without the client
needing to perform manual casting.

**Author:** Mario Galindo Queralt, Ph.D.
