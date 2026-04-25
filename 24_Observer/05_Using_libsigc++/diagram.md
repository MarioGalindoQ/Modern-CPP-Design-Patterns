# Observer Pattern (Using libsigc++)

```mermaid
classDiagram
    class sigc_trackable {
        <<Library>>
    }

    class sigc_signal {
        <<Library>>
        +connect() connection
        +emit()
    }

    class sigc_connection {
        <<Library>>
        +disconnect()
    }

    class Observer~T~ {
        <<interface>>
        +update(T&)*
    }

    class Subject~T~ {
        -sigc_signal observers
        +attach(Observer&) connection
        #notify()
    }

    class Number {
        -int n
        +setVal(int)
        +getVal() int
    }

    class DivObserver {
        -int div
        +update(Number&)
    }

    class ModObserver {
        -int div
        +update(Number&)
    }

    class Client {
        +main()
    }

    %% Inheritance (Is_a)
    sigc_trackable <|-- Observer~T~
    Observer~T~ <|.. DivObserver
    Observer~T~ <|.. ModObserver
    Subject~T~ <|-- Number

    %% Composition (Has_a)
    Subject~T~ *-- sigc_signal : observers

    %% Dependencies
    sigc_signal ..> "n" Observer~T~ : triggers update
    Client ..> sigc_connection : manages link
    Client ..> Number
    Client ..> Observer~T~
```

### Design Note:
This version leverages 'libsigc++' to implement a Signals & Slots mechanism.

1. Automatic Safety: By inheriting from 'sigc::trackable', observers are
automatically disconnected from the signal when they are destroyed, preventing
crashes.
2. Connection Management: The 'sigc::connection' object returned by 'attach'
(connect) allows the client to manually break the link at any time without
involving the subject directly.
3. Clean Handshake: The use of 'sigc::mem_fun' binds the concrete observer's
method to the signal, maintaining full type safety across the board.

**Author:** Mario Galindo Queralt, Ph.D.
