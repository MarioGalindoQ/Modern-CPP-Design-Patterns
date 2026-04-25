# Strategy Pattern (Simple Version)

```mermaid
classDiagram
    class SortStrategy {
        <<interface>>
        +sort(vector~int~& data)*
    }

    class BubbleSort {
        +sort(vector~int~& data)
    }

    class ShellSort {
        +sort(vector~int~& data)
    }

    class Sorter {
        -unique_ptr~SortStrategy~ strategy_
        +setStrategy(unique_ptr~SortStrategy~)
        +sortVector(vector~int~& data)
    }

    class Client {
        +main()
    }

    %% Inheritance (Is_a)
    SortStrategy <|.. BubbleSort
    SortStrategy <|.. ShellSort

    %% Composition (Has_a)
    Sorter *-- SortStrategy : strategy_

    %% Dependency - No numbers
    Client ..> Sorter
    Client ..> BubbleSort
    Client ..> ShellSort
```

### Design Note:
In this simple implementation, the 'Sorter' class acts as the Context. It does
not know the details of the sorting algorithms; it only maintains a reference to
the 'SortStrategy' interface. The Client is responsible for injecting the
desired concrete strategy (BubbleSort or ShellSort) into the context at
runtime. This fulfills the Open/Closed Principle, as new sorting algorithms can
be added without modifying the existing 'Sorter' code.

**Author:** Mario Galindo Queralt, Ph.D.
