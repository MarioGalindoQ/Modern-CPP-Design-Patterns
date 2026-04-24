# Template Method Pattern

```mermaid
classDiagram
   class SortAndPrint {
      <<abstract>>
      +sortAndPrintVector(vector~int~&)
      -sort(vector~int~&)*
      -print(vector~int~&)*
   }

   class BubbleSort {
      -sort(vector~int~&)
      -print(vector~int~&)
   }

   class ShellSort {
      -sort(vector~int~&)
      -print(vector~int~&)
   }

   class Client {
      +main()
   }

   %% Inheritance (Is_a)
   SortAndPrint <|.. BubbleSort
   SortAndPrint <|.. ShellSort

   %% Dependency
   Client ..> SortAndPrint
   Client ..> BubbleSort
   Client ..> ShellSort
```

### Design Note:
In the Template Method pattern, the base class 'SortAndPrint' defines the
invariant part of the algorithm (the skeleton) in the public
'sortAndPrintVector' method. The variant parts (the specific sorting and
printing logic) are defined as private virtual placeholders that the base class
calls. This follows the Hollywood Principle: "Don't call us, we will call
you". Subclasses provide the implementation details, but the base class
maintains control over the execution order.
