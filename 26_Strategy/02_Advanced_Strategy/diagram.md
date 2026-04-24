# Strategy Pattern (Advanced Composition)

```mermaid
classDiagram
   class SortStrategy {
      <<interface>>
      +sort(vector~int~&)*
   }

   class BubbleSort {
      +sort(vector~int~&)
   }

   class ShellSort {
      +sort(vector~int~&)
   }

   class PrintStrategy {
      <<interface>>
      +print(vector~int~&)*
   }

   class FreePrint {
      +print(vector~int~&)
   }

   class WidthPrint {
      +print(vector~int~&)
   }

   class SortAndPrint {
      -unique_ptr~SortStrategy~ sortImpl_
      -unique_ptr~PrintStrategy~ printImpl_
      +setSort(unique_ptr~SortStrategy~)
      +setPrint(unique_ptr~PrintStrategy~)
      +execute(vector~int~&)
   }

   class Client {
      +main()
   }

   SortStrategy <|.. BubbleSort
   SortStrategy <|.. ShellSort

   PrintStrategy <|.. FreePrint
   PrintStrategy <|.. WidthPrint

   SortAndPrint *-- SortStrategy : sortImpl_
   SortAndPrint *-- PrintStrategy : printImpl_

   Client ..> SortAndPrint
   Client ..> BubbleSort
   Client ..> ShellSort
   Client ..> FreePrint
   Client ..> WidthPrint
```

### Design Note:
In this advanced version, the Context ('SortAndPrint') delegates its execution
to multiple, independent strategy families simultaneously (sorting and
printing). By using Dependency Injection, the Client configures the Context at
runtime. This prevents a combinatorial explosion of subclasses (e.g.,
BubbleSortWithFreePrint, ShellSortWithWidthPrint) and demonstrates that the
Strategy pattern scales perfectly through composition.
