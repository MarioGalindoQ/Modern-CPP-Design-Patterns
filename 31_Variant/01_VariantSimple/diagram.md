# std::variant (Modern C++17 Feature)

```mermaid
classDiagram
   class Logger {
      +string s
      +Logger_ RuleOfSeven
   }

   class Visitor {
      +int counts
      +visit_int()
      +visit_Logger()
      +visit_string()
   }

   class var_t {
   }

   class Client {
      +main()
   }

   %% Relationships
   var_t ..> Logger
   var_t ..> int
   var_t ..> string

   Visitor ..> var_t : processes

   %% The Client owns a collection of variants by value
   Client *-- "n" var_t : stack

   %% Dependency for manual access
   Client ..> var_t : std_get_if
```

### Design Note:
This diagram represents a non-polymorphic heterogeneous collection. The 'var_t'
(std::variant) acts as a type-safe union that can hold any of the specified
types. The 'Visitor' is a stateful functor that performs operations on the
variant's current content via 'std::visit'. Unlike traditional OO designs, there
is no base class; memory is managed automatically on the stack or within the
container, following Value Semantics. The 'Logger' class is used here to prove
that constructors and destructors are called correctly during these value-based
transitions.

**Author:** Mario Galindo Queralt, Ph.D.
