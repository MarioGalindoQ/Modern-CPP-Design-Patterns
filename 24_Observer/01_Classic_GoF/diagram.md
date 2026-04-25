# Observer Pattern (Classic GoF Version)

```mermaid
classDiagram
    class Observable {
        -vector~Observer*~ observers_
        +attach(Observer*)
        +detach(Observer*)
        #notify()
    }

    class Observer {
        <<interface>>
        +update(Observable*)*
    }

    class Number {
        -int val_
        +setVal(int)
        +getVal() int
    }

    class DivObserver {
        -int div_
        +update(Observable*)
    }

    class ModObserver {
        -int div_
        +update(Observable*)
    }

    class Client {
        +main()
    }

    %% Inheritance (Is_a) - No numbers
    Observable <|-- Number
    Observer <|.. DivObserver
    Observer <|.. ModObserver

    %% Composition (Has_a) - Multiplicity "n" only at the end
    Observable *-- "n" Observer : observers_

    %% Dependency - No numbers
    Observer ..> Observable : updates from
    Client ..> Number
    Client ..> Observer
```

### Design Note:
In this classic version, the 'Observable' class maintains a collection of
'Observer' pointers. When a state change occurs, the 'notify()' method iterates
through the list and calls 'update(this)'. Since the 'Observer' interface only
knows about the base 'Observable' class, concrete observers like 'DivObserver'
must cast the pointer back to 'Number' to retrieve the current value.

**Author:** Mario Galindo Queralt, Ph.D.
