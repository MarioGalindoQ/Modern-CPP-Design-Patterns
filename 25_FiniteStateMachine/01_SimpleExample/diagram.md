# Finite State Machine (Simple Example)

```mermaid
classDiagram
    class IState_FSM {
        <<interface>>
        +onEntry(FSM*)
        +onExit(FSM*)
        +event_stay(FSM*)
        +event_change(FSM*)
        +event_only_A(FSM*)
    }

    class State_A {
        +onEntry(FSM*)
        +onExit(FSM*)
        +event_stay(FSM*)
        +event_change(FSM*)
        +event_only_A(FSM*)
    }

    class State_B {
        +onEntry(FSM*)
        +onExit(FSM*)
        +event_stay(FSM*)
        +event_change(FSM*)
    }

    class FSM {
        -static stateMap_
        -IState_FSM* state_
        +string name
        +int i
        +changeState(string, Action)
        +event_stay()
        +event_change()
        +event_only_A()
    }

    class Client {
        +main()
    }

    %% Inheritance (Is_a) - No numbers
    IState_FSM <|.. State_A
    IState_FSM <|.. State_B

    %% The FSM owns all available states in a static registry
    %% Composition (Has_a) - Multiplicity "n" at the end
    FSM *-- "n" IState_FSM : stateMap_

    %% The States depend on the FSM to trigger transitions
    %% Dependency (..>) - No numbers
    State_A ..> FSM : changeState()
    State_B ..> FSM : changeState()

    %% Client interacts with the Context
    Client ..> FSM
```

### Design Note:
This simple FSM demonstrates the basic interaction between the 'Context' (FSM)
and the 'State' objects. The FSM delegates all event handling to the 'state_'
pointer. When a transition is needed, the concrete state class calls
'changeState()' on the FSM, which handles the exit/entry logic. The states are
managed by a static map, ensuring that each state class is instantiated only
once (Singleton/Flyweight behavior).
