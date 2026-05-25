# Component Registry / Data-Oriented Design Diagram

```mermaid
classDiagram
   class Registry {
      -uint32_t nextEntityId_
      -static inline vectors
      +getInstance() Registry&
      +createEntity() EntityBuilder
      +getEntityName(uint32_t) string
      +addComponent(T&&)
      +getComponents() vector~T~&
   }

   class EntityBuilder {
      -uint32_t id_
      -string name_
      -float x, y, vx, vy
      -int state_
      +setName(string) EntityBuilder&
      +setPosition(float, float) EntityBuilder&
      +setVelocity(float, float) EntityBuilder&
      +setAIState(int) EntityBuilder&
      +build() uint32_t
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

   class PhysicsSystem {
      +update()
   }

   class AISystem {
      +update()
   }

   class ScenarioSystem {
      +update(int)
   }

   class Client {
      +main()
   }

   %% The Registry owns the contiguous data buckets
   Registry *-- "n" Position
   Registry *-- "n" Velocity
   Registry *-- "n" AIControl
   Registry *-- "n" Label

   %% The Builder is the factory for aligned entities
   Registry ..> EntityBuilder : creates
   EntityBuilder ..> Registry : pushes data to
   EntityBuilder ..> Position : initializes
   EntityBuilder ..> Velocity : initializes

   %% Systems use the Registry to access or mutate data
   PhysicsSystem ..> Registry : requests data
   AISystem ..> Registry : requests data
   ScenarioSystem ..> Registry : mutates state

   %% Client orchestrates the simulation
   Client --> Registry : accesses
   Client ..> EntityBuilder : configures entities
   Client --> PhysicsSystem : executes
   Client --> AISystem : executes
   Client --> ScenarioSystem : manages timeline
```

### Design Note:
This diagram illustrates the complete Data-Oriented Design (DOD) architecture. 
The 'Registry' (Meyers Singleton) acts as the central owner of all component 
vectors. A Fluent 'EntityBuilder' is introduced to guarantee 'Parallel Array 
Alignment': it ensures that every time an entity is created, all corresponding 
data buckets are updated simultaneously, allowing 'Systems' to use a shared 
index 'i' for O(1) cross-component access. This layout maximizes CPU cache 
efficiency by enabling sequential access patterns and eliminating virtual 
function overhead.

**Author:** Mario Galindo Queralt, Ph.D.
