# Null Object Pattern (Classic GoF)

```mermaid
classDiagram
   class Logger {
      <<interface>>
      +log(string)*
   }

   class ConsoleLogger {
      -string name_
      +log(string)
   }

   class NullLogger {
      -NullLogger()
      +getInstance() Logger&
      +log(string)
   }

   class Client {
      +main()
   }

   %% Inheritance (Is_a) - No numbers
   Logger <|.. ConsoleLogger
   Logger <|.. NullLogger

   %% Singleton relationship for the Null Object
   NullLogger *-- NullLogger : 1 static instance

   %% Client depends on the interface
   Client ..> Logger
```

### Design Note:
In this classic implementation, 'NullLogger' is a concrete class that inherits
from the 'Logger' interface but provides a "no-op" (do nothing) implementation
of the 'log' method. By providing a valid object instead of a null pointer, the
'Client' can iterate through a collection of loggers and call 'log()' without
using 'if (logger != nullptr)' checks, leading to cleaner and safer code.

**Author:** Mario Galindo Queralt, Ph.D.
