# Memento Pattern (String Serialization Version)

```mermaid
classDiagram
    class ComponentA {
        -string state_
        +setState(string)
        +serialize() string
        +deserialize(string)
    }

    class ComponentB {
        -int value_
        +setValue(int)
        +serialize() string
        +deserialize(string)
    }

    class Caretaker {
        -stack~string~ history_
        +save(string)
        +undo() string
        +saveToFile(string)
        +restoreFromFile(string)
    }

    class DiskFile {
        <<System>>
        memento_stack_data.txt
    }

    class Client {
        +main()
    }

    %% Persistence relationship
    Caretaker ..> DiskFile : saves to
    Caretaker ..> DiskFile : restore from

    %% The Caretaker owns a collection of serialized states
    Caretaker *-- "n" string : history_

    %% Originators depend on strings to export/import state
    ComponentA ..> string : creates/reads
    ComponentB ..> string : creates/reads

    %% Client manages the workflow
    Client ..> Caretaker : manages history
    Client ..> ComponentA
    Client ..> ComponentB
```

### Design Note:
In this modern pragmatic version, the Memento is simplified into a
'std::string'. The 'Caretaker' acts as a persistence manager, holding a stack of
these strings in memory and providing the ability to dump them into a
'DiskFile'. The simulation demonstrates a "Fresh Restart" where the memory is
cleared and the state is perfectly reconstructed from the serialized data on
disk.

**Author:** Mario Galindo Queralt, Ph.D.
