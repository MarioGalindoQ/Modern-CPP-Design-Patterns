# Singleton Pattern (Simple)

```mermaid
classDiagram
    class Singleton {
        -Singleton()
        -~Singleton()
        -Singleton(const Singleton&)
        +getInstance() Singleton&
        +talk()
    }

    Singleton *-- Singleton : 1 Static instance
```

**Author:** Mario Galindo Queralt, Ph.D.
