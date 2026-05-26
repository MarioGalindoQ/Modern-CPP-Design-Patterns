# Type Erasure Pattern (Task Queue)

```mermaid
classDiagram
   class AnyTask {
      -unique_ptr~StorageInterface~ pimpl_
      +AnyTask(BusinessType&&)
      +AnyTask(const AnyTask&)
      +run()
   }

   class StorageInterface {
      <<interface>>
      +execute_v()*
      +clone_v()* unique_ptr~StorageInterface~
   }

   class Model~BusinessType~ {
      -BusinessType function_
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
   AnyTask *-- "1" StorageInterface : pimpl_

   %% The Model implements the StorageInterface
   Model~BusinessType~ --|> StorageInterface : Is_a

   %% The Model owns the actual callable logic
   Model~BusinessType~ *-- "1" BusinessType : function_

   %% Various types can be used to instantiate the Model
   Model~BusinessType~ ..> LogWorker : instantiated with
   Model~BusinessType~ ..> Lambda : instantiated with

   %% The Client manages the task queue by value
   Client *-- "n" AnyTask : taskQueue
```

### Design Note:
This diagram shows the Type Erasure pattern applied to a task execution 
system. The 'AnyTask' class acts as a value-based container that erases 
the specific type of any callable object (like a Lambda or a custom 
functor class). 
1. The 'StorageInterface' provides the virtual polymorphic interface.
2. The 'Model' template bridges the gap by wrapping the specific type.
3. The 'Client' can store these heterogeneous tasks in a single 
   'std::vector<AnyTask>' without using pointers or inheritance, 
   ensuring high cohesion and safe memory management through the 
   'Rule of Seven'.

**Author:** Mario Galindo Queralt, Ph.D.
