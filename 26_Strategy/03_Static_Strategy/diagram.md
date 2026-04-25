# Strategy Pattern (Static / Mixin Inheritance)

```mermaid
classDiagram
    class BubbleSort {
        +sort(vector~int~& data)
    }

    class ShellSort {
        +sort(vector~int~& data)
    }

    class FreePrint {
        +print(vector~int~& data)
    }

    class WidthPrint {
        +print(vector~int~& data)
    }

    class SortAndPrint~SortAlgo, PrintAlgo~ {
        +execute(vector~int~& data)
    }

    class Client {
        +main()
    }

    %% Inheritance (Is_a) via Mixin Inheritance
    %% The Context inherits from its template arguments
    SortAlgo <|-- SortAndPrint~SortAlgo, PrintAlgo~
    PrintAlgo <|-- SortAndPrint~SortAlgo, PrintAlgo~

    %% Client defines the types at compile-time
    Client ..> SortAndPrint~SortAlgo, PrintAlgo~
    Client ..> BubbleSort
    Client ..> ShellSort
    Client ..> FreePrint
    Client ..> WidthPrint
```

### Design Note:
In this static version, the 'SortAndPrint' class uses Mixin Inheritance. It
inherits from its template parameters ('SortAlgo' and 'PrintAlgo'). There is no
'Has_a' relationship (Composition) because there are no pointers; the object is
a single, flat structure in memory. This allows the compiler to perform
aggressive inlining, achieving Zero-Overhead polymorphism. The specific behavior
is "baked into" the type itself during compilation.

**Author:** Mario Galindo Queralt, Ph.D.
