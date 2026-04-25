# Memento Pattern (GoF Version)

```mermaid
classDiagram
    class Memento {
        <<interface>>
        +~Memento()
    }

    class Originator {
        <<interface>>
        +createMemento()* MementoPtr
        +restoreMemento(MementoPtr)*
    }

    class Caretaker {
        -stack~MementoPtr~ history_
        +save(MementoPtr)
        +undo() MementoPtr
        +discardLatest()
    }

    class ComponentA {
        -string state_
        +setState(string)
        +createMemento() MementoPtr
        +restoreMemento(MementoPtr)
    }

    class MementoA {
        +string savedState
    }

    class ComponentB {
        -int value_
        +setValue(int)
        +createMemento() MementoPtr
        +restoreMemento(MementoPtr)
    }

    class MementoB {
        +int savedValue
    }

    %% Inheritance (Is_a)
    Originator <|.. ComponentA
    Originator <|.. ComponentB
    Memento <|.. MementoA
    Memento <|.. MementoB

    %% Caretaker owns a collection of Mementos
    Caretaker *-- "n" Memento : history_

    %% Relationship between Originators and their specific Mementos
    %% In C++ these are nested, shown here as dependency/creation
    ComponentA ..> MementoA : creates/reads
    ComponentB ..> MementoB : creates/reads

    %% Client uses the Caretaker to manage history
    Client ..> Caretaker
    Client ..> Originator
```

### Design Note:
In the Classic GoF version, encapsulation is key. 'ComponentA' and 'ComponentB'
are the only ones capable of seeing inside their respective Mementos ('MementoA'
and 'MementoB'). The 'Caretaker' treats all mementos as opaque 'Memento' base
pointers, fulfilling its role as a safekeeper without violating the internal
state of the components.

**Author:** Mario Galindo Queralt, Ph.D.
