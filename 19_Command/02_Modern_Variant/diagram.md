# Command Pattern (Modern Variant)

```mermaid
classDiagram
    class CowCommand {
        <<empty struct>>
    }
    class DogCommand {
        <<empty struct>>
    }
    class CarCommand {
        <<empty struct>>
    }

    class Command {
        <<variant>>
    }

    class Cow {
        +moo()
    }

    class Dog {
        +bark()
    }

    class Car {
        +turnOn()
        +turnOff()
        +rev()
    }

    class CommandExecutor {
        -Cow cow
        -Dog dog
        -Car car
        +operator()(CowCommand)
        +operator()(DogCommand)
        +operator()(CarCommand)
    }

    class CommandQueue {
       -vector~Command~ queue_
       -CommandExecutor& commandExecutor_
       +addCommand(Command) void
        +runAll() void
    }

    class Client {
        +main()
    }

    CommandQueue *-- "n" Command : queue_
    CommandQueue *-- CommandExecutor : commandExecutor_

    Command ..> CowCommand
    Command ..> DogCommand
    Command ..> CarCommand

    CommandExecutor *-- Cow
    CommandExecutor *-- Dog
    CommandExecutor *-- Car

    Client ..> CommandQueue
```

### Design Note:

In this modern approach, the 'Command' is a type-safe union (std::variant) of
plain structs. The 'CommandExecutor' holdsthe receivers,
establishing a 'Has_a' relationship (Composition). The Command Queue manages a
collection of commands by value and depends on the Executor to process them via
'std::visit'.

**Author:** Mario Galindo Queralt, Ph.D.
