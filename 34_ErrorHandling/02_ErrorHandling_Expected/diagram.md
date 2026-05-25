# Error Handling (std::expected Version)

```mermaid
classDiagram
   class Error {
      +string message
   }

   class expected_double_Error {
      +has_value() bool
      +value() double
      +error() Error
      +operator*() double
   }

   class Client {
      +safeSqrt(string) expected
      +main()
   }

   %% Relationships using your defined symbology:

   expected_double_Error ..> Error
   expected_double_Error ..> double

   Client ..> expected_double_Error
   Client ..> Error
```

### Design Note:
This is the state-of-the-art C++23 approach for error handling. The
'std::expected' template acts as a vocabulary type that clearly communicates
that a function might fail. Unlike 'std::variant', it provides a pointer-like
interface (operator* and bool conversion) which is more intuitive for C++
developers. This implementation represents the final evolution in the
repository, moving from manual error management to a standardized,
high-performance, and type-safe functional approach.

**Author:** Mario Galindo Queralt, Ph.D.
