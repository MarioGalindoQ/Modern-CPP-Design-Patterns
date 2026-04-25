# CRTP: Object Counter

```mermaid
classDiagram
   class ObjectCounter~T~ {
      -static int createdCount_
      -static int aliveCount_
      #ObjectCounter()
      #~ObjectCounter()
      +getCreated() int
      +getAlive() int
   }

   class Dragon {
      +roar()
   }

   class Elephant {
      +trumpet()
   }

   class Client {
      +main()
      +printSummary()
   }

   %% Inheritance (Is_a) via CRTP
   ObjectCounter~Dragon~ <|-- Dragon
   ObjectCounter~Elephant~ <|-- Elephant

   %% Client uses the concrete classes and their static interfaces
   Client ..> Dragon
   Client ..> Elephant
```

### Design Note:
This diagram shows the "Object Counter" idiom. By using CRTP, the
'ObjectCounter' base class is instantiated separately for 'Dragon' and
'Elephant'. This means the static members 'createdCount_' and 'aliveCount_' are
not shared between different species; each derived class gets its own unique set
of counters automatically. The protected constructor and destructor in the base
class ensure that the counting logic is triggered every time a derived object is
born or dies.

**Author:** Mario Galindo Queralt, Ph.D.
