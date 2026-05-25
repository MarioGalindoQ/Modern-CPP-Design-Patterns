# Component Registry / Data-Oriented Design Diagram

```mermaid
classDiagram
   class Registry {
      -uint32_t nextEntityId_
      -static inline vectors
      +getInstance() Registry&
      +createEntity(string) uint32_t
      +getEntityName(uint32_t) string
      +addComponent(T&&)
      +getComponents() vector~T~&
   }

   class Position {
      +uint32_t entityId
      +float x
      +float y
   }

   class Velocity {
      +uint32_t entityId
      +float vx
      +float vy
   }

   class AIControl {
      +uint32_t entityId
      +int state
   }

   class Label {
      +uint32_t entityId
      +string name
   }

   class MovementSystem {
      +update()
   }

   class AISystem {
      +update()
   }

   class ScenarioSystem {
      +update(int, uint, uint)
   }

   class Client {
      +main()
   }

   %% The Registry owns the contiguous data buckets
   Registry *-- "n" Position
   Registry *-- "n" Velocity
   Registry *-- "n" AIControl
   Registry *-- "n" Label

   %% Systems use the Registry to access or mutate data
   MovementSystem ..> Registry : requests data
   AISystem ..> Registry : requests data
   ScenarioSystem ..> Registry : mutates state

   %% Client orchestrates the simulation
   Client --> Registry : accesses
   Client --> MovementSystem : executes
   Client --> AISystem : executes
   Client --> ScenarioSystem : manages timeline
```

### Design Note:
This diagram illustrates the Data-Oriented Design (DOD) architecture. The 
'Registry' (Meyers Singleton) acts as the central owner of all component 
vectors, ensuring that data of the same type is stored contiguously in 
memory. The 'Systems' are decoupled from each other and from the entities 
themselves; they only interact with the Registry to pull or mutate specific 
data buckets. This layout maximizes CPU cache efficiency by enabling 
sequential access patterns and eliminating virtual function overhead.

**Author:** Mario Galindo Queralt, Ph.D.
