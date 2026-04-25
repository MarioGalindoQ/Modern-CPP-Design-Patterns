# Memento Pattern (Persistent Mmap Version)

```mermaid
classDiagram
    class SystemState {
        <<struct>>
        +int valB
        +char strA[64]
    }

    class FileHeader {
        <<struct>>
        +size_t count
        +size_t cursor
    }

    class ComponentA {
        -string state_
        -Caretaker& caretaker_
        +setState(string)
        +internalSet(string)
    }

    class ComponentB {
        -int value_
        -Caretaker& caretaker_
        +setValue(int)
        +internalSet(int)
    }

    class Caretaker {
        -FileHeader* header_
        -SystemState* history_
        +save()
        +undo()
        +redo()
        +setComponents(A, B)
    }

    class BinaryFile {
        <<System>>
        memento_history.bin
    }

    class Client {
        +main()
    }

    %% The Caretaker owns the structures mapped in memory
    Caretaker *-- FileHeader : metadata
    Caretaker *-- "n" SystemState : history_

    %% Components interact with the Caretaker for Auto-Save
    ComponentA o-- Caretaker
    ComponentB o-- Caretaker

    %% Caretaker manages the physical storage
    Caretaker ..> BinaryFile : maps to memory

    %% Client manages the workflow and crash simulation
    Client ..> Caretaker : manages life cycle
    Client ..> ComponentA
    Client ..> ComponentB
    Client ..> BinaryFile : survives crash
```

### Design Note:
This version implements a **High-Performance Persistent Memento**. 
The `Caretaker` maps a `BinaryFile` directly into memory using `mmap`. 
1. **Auto-Save**: `ComponentA` and `ComponentB` hold a reference to the 
   `Caretaker` and trigger a `save()` automatically on every mutation. 
2. **Binary Snapshots**: Each state is a POD `SystemState` struct containing 
   the full system data, ensuring atomic restores. 
3. **Crash Resilience**: Because the `FileHeader` (with the cursor) and the 
   `history_` are stored on disk, the system can perfectly recover its latest 
   state after a program crash, allowing for continued Undo/Redo operations.

**Author:** Mario Galindo Queralt, Ph.D.
