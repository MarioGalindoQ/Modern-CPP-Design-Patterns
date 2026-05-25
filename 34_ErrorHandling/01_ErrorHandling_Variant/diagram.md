# Error Handling (std::variant Version)

```mermaid
classDiagram
   class Error {
      +string message
   }

   class Result {
      <<variant>>
   }

   class ResultHandler {
      +visit_double()
      +visit_Error()
   }

   class Client {
      +safeSqrt(string) Result
      +main()
   }

   %% Relationships
   Result ..> Error : may contain
   Result ..> double : may contain

   ResultHandler ..> Result : processes
   ResultHandler ..> Error : handles

   Client ..> Result : creates
   Client ..> ResultHandler : uses
```

### Design Note:
This diagram illustrates the "Errors as Values" paradigm. The 'Result' type is a
type-safe union (std::variant) that forces the developer to acknowledge the
possibility of failure. The 'ResultHandler' visitor encapsulates the logic for
both successful and failed outcomes. This design eliminates the need for output
parameters or side-effect-heavy exceptions, making the function 'safeSqrt'
honest about its potential results.

**Author:** Mario Galindo Queralt, Ph.D.
