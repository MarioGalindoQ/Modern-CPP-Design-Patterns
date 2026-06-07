# Type Erasure Pattern (Task Queue)

```mermaid
classDiagram
   class AnyTaskEntity {
      -unique_ptr~StorageInterface~ pimpl_
      +AnyTaskEntity(TaskType&&)
      +AnyTaskEntity(const AnyTaskEntity&)
      +run()
   }

   class StorageInterface {
      <<interface>>
      +execute_v()*
      +clone_v()* unique_ptr~StorageInterface~
   }

   class Model~TaskType~ {
      -TaskType function_
      +execute_v()
      +clone_v() unique_ptr~StorageInterface~
   }

   class LogWorker {
      -string id_
      +operator()()
   }

   class Lambda {
      <<anonymous>>
      +operator()()
   }

   class Client {
      +main()
   }

   %% The Wrapper owns the internal StorageInterface
   AnyTaskEntity *-- "1" StorageInterface : pimpl_

   %% The Model implements the StorageInterface
   Model~TaskType~ --|> StorageInterface : Is_a

   %% The Model owns the actual callable logic
   Model~TaskType~ *-- "1" TaskType : function_

   %% Various types can be used to instantiate the Model
   Model~TaskType~ ..> LogWorker : instantiated with
   Model~TaskType~ ..> Lambda : instantiated with

   %% The Client manages the task queue by value
   Client *-- "n" AnyTaskEntity : taskQueue
```

### Design Note:
This diagram shows the Type Erasure pattern applied to a task execution 
system. The 'AnyTaskEntity' class acts as a value-based container that erases 
the specific type of any callable object (like a Lambda or a custom 
functor class). 
1. The 'StorageInterface' provides the virtual polymorphic interface.
2. The 'Model' template bridges the gap by wrapping the specific type.
3. The 'Client' can store these heterogeneous tasks in a single 
   'std::vector<AnyTaskEntity>' without using pointers or inheritance, 
   ensuring high cohesion and safe memory management through the 
   'Rule of Seven'.

**Author:** Mario Galindo Queralt, Ph.D.
