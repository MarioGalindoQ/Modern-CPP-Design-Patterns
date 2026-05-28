# Polymorphic Bottleneck Analysis Diagram

```mermaid
classDiagram
   class Base {
      <<abstract>>
      +concrete() int
      +virt()* int
   }

   class DerivedA {
      +virt() int
   }

   class DerivedB {
      +virt() int
   }

   class Client {
      +main()
   }

   %% Inheritance: Is_a relationship
   Base <|-- DerivedA
   Base <|-- DerivedB

   %% Composition: The Client (main) owns a collection of objects
   Client *-- "n" Base : chaotic_objects

   %% Dependency: Client calls methods on the hierarchy
   Client ..> Base : benchmarks calls
```

### Design Note:
This diagram represents the classical Object-Oriented (OOP) structure used in 
the benchmark. The 'Client' (main function) manages a heterogeneous 
collection of 'Base' pointers, interleaved with 'DerivedA' and 'DerivedB' 
instances. This setup is designed to trigger hardware branch mispredictions 
within the CPU's Branch Target Buffer (BTB) when the virtual function 
'virt()' is called on a shuffled vector. Unlike the Data-Oriented approach 
in Example 02, this design prioritizes abstraction over memory locality, 
resulting in the measured performance bottleneck.

**Author:** Mario Galindo Queralt, Ph.D.
