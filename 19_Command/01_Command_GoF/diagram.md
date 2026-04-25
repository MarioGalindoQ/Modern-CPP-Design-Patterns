# Command Pattern (GoF Version)

```mermaid
classDiagram
   class ICommand {
      <<interface>>
      +execute()*
   }

   class CowCommand {
      -Cow& receiver_
      +execute()
   }

   class DogCommand {
      -Dog& receiver_
      +execute()
   }

   class CarCommand {
      -Car& receiver_
      +execute()
   }

   class Cow {
      +moo()
   }

   class Dog {
      -string name_
      +bark()
   }

   class Car {
      -bool engineOn_
      +turnOn()
      +turnOff()
      +rev()
   }

   class CommandQueue {
      -vector~unique_ptr~ICommand~~ queue_
      +addCommand(unique_ptr~ICommand~)
      +runAll()
   }

   class Client {
      +main()
   }

   %% Inheritance (Implements)
   ICommand <|.. CowCommand
   ICommand <|.. DogCommand
   ICommand <|.. CarCommand

   %% The CommandQueue owns the commands
   %% Composition (Has_a) - Multiplicity "n" at the end
   CommandQueue *-- "n" ICommand : queue_

   %% Commands depend on their Receivers to do the work
   CowCommand o-- Cow : receiver_
   DogCommand o-- Dog : receiver_
   CarCommand o-- Car : receiver_

   %% Client orchestrates everything
   Client *-- CommandQueue

   %% Client is the owner of the Receivers
   Client *-- Cow
   Client *-- Dog
   Client *-- Car
```

### Design Note:
In this traditional version, each 'Command' object acts as a bridge. It knows
which 'Receiver' method to call. The 'CommandQueue' remains completely
decoupled from the 'Receivers', as it only interacts with the 'Command'
interface to trigger actions.

**Author:** Mario Galindo Queralt, Ph.D.
