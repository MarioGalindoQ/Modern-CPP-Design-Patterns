# Registry Pattern

```mermaid
classDiagram
   class Image

   class Processor {
      <<interface>>
      +process(image) image*
   }

   class GrayscaleProcessor {
      +process(image) image
   }

   class BlurProcessor {
      +process(image) image
   }

   class Registry {
      -static registryMap_
      +registerProcessor(string processorName, processorCreator)
      +create(string processorName) shared_ptr~Processor~
   }

   class Register~ConcreteProcessor~ {
      +Register(string processorName)
   }

   class Client {
      +image
      +main()
   }

   %% Inheritance (Implements)
   Processor <|.. GrayscaleProcessor
   Processor <|.. BlurProcessor

   %% The Registry manages the lifecycle of processors (Singleton/Flyweight)
   %% Composition (Has_a) - Multiplicity "n" at the end
   Registry *-- "n" Processor : manages

   %% Auto-registration mechanism
   %% Dependency (..>) - No numbers
   Register~ConcreteProcessor~ ..> Registry : registers ConcreteProcessor
   GrayscaleProcessor ..> Register~ConcreteProcessor~ : static auto-reg
   BlurProcessor ..> Register~ConcreteProcessor~ : static auto-reg

   %% Client interactions
   Client *-- Image
   Client ..> Registry : call create(processorName)
   Client o-- "n" Processor
   Client ..> Processor : process(image)
```

### Design Note:
This diagram highlights the decoupled nature of the Registry. The 'Client' has
no direct link to 'GrayscaleProcessor' or 'BlurProcessor'. The 'Register' helper
facilitates a "push" registration during static initialization, populating the
Registry's internal map before the 'main' function starts.

**Author:** Mario Galindo Queralt, Ph.D.
