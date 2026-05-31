# Expression Templates: Traditional Naive Approach

```mermaid
classDiagram
   class Vector {
      -vector~double~ data_
      +Vector(size)
      +operator[](size_t) double
      +operator+(Vector, Vector) Vector
      +operator*(double, Vector) Vector
   }

   class Client {
      +main()
   }

   %% Operators create and return new full-sized instances
   Vector ..> "1" Vector : creates temporary

   %% Client manages vectors
   Client *-- "n" Vector : manages
   Client ..> Vector : executes operations
```

### Design Note:
This diagram illustrates the traditional operator overloading approach. Each mathematical 
operation (addition or scaling) is a "greedy" function that immediately calculates 
the result and returns a new 'Vector' instance. This leads to a chain of temporary 
objects being allocated on the heap, forcing the CPU to perform multiple passes 
over the memory.

**Author:** Mario Galindo Queralt, Ph.D.
