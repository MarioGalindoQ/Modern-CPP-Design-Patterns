# Finite State Machine (CD Player - GoF Version)

```mermaid
classDiagram
   class Singleton {
      -static singletonMap
      #getSingleton(string)
   }

   class IState_FSM {
      <<interface>>
      +onEntry(FSM*)
      +onExit(FSM*)
      +open(FSM*)
      +close(FSM*)
      +play(FSM*)
      +stop(FSM*)
      +pause(FSM*)
      +insert_CD(FSM*, int)
      +remove_CD(FSM*)
   }

   class Closed_without_CD { +open(FSM*) }
   class Open_without_CD   { +close(FSM*), +insert_CD(FSM*, int) }
   class Open_with_CD      { +close(FSM*), +remove_CD(FSM*), +play(FSM*) }
   class Closed_with_CD    { +open(FSM*), +play(FSM*) }
   class Playing           { +stop(FSM*), +pause(FSM*), +next_song(FSM*), ... }
   class Paused            { +play(FSM*), +stop(FSM*) }

   class FSM {
      -IState_FSM* state_
      +string name
      +bool tray_open
      +bool playing
      +bool cian_light_on
      +int nSongs
      +changeState(string, Action)
      +open()
      +play()
      +insert_CD(int)
      ...
   }

   class Client {
      +main()
   }

   %% Hierarchy
   Singleton <|.. IState_FSM
   IState_FSM <|.. Closed_without_CD
   IState_FSM <|.. Open_without_CD
   IState_FSM <|.. Open_with_CD
   IState_FSM <|.. Closed_with_CD
   IState_FSM <|.. Playing
   IState_FSM <|.. Paused

   %% Relationships
   %% The FSM has a pointer to the current state (Composition)
   FSM *-- "1" IState_FSM : current state
   
   %% The Singleton base manages all state instances (Composition)
   Singleton *-- "n" IState_FSM : registry

   %% States use the FSM context to change data and trigger transitions
   Closed_without_CD ..> FSM
   Open_without_CD ..> FSM
   Playing ..> FSM

   Client ..> FSM
```

### Design Note:
In this classic GoF implementation, the 'CD Player' (FSM) is decoupled from its
behavior. Each class represents a physical state of the hardware. The use of
'Singleton' as a base for 'IState_FSM' ensures that only one instance of each
state exists in memory (Flyweight). The FSM maintains a pointer to the current
state and delegates all external events (buttons, sensors) to it. Transitions
are performed by the states themselves calling 'changeState' back on the FSM
context.
