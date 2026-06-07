# Type Erasure Pattern (Non-Intrusive Serialization)

```mermaid
classDiagram
   class SerializableEntity {
      -unique_ptr~StorageInterface~ pimpl_
      +SerializableEntity(BusinessType&&)
      +SerializableEntity(const SerializableEntity&)
      +operator=(const SerializableEntity&)
      +save(ostream&) const
   }

   class StorageInterface {
      <<interface>>
      +serialize_v(ostream&)*
      +clone_v()* unique_ptr~StorageInterface~
   }

   class Model~BusinessType~ {
      -BusinessType data_
      +serialize_v(ostream&)
      +clone_v() unique_ptr~StorageInterface~
   }

   class User {
      -string name_
      +serialize(ostream&) const
   }

   class NetworkConfig {
      -int port_
      +serialize(ostream&) const
   }

   class Client {
      +main()
   }

   %% Relationships using your symbology reference:

   %% The Wrapper owns the internal StorageInterface
   SerializableEntity *-- "1" StorageInterface : pimpl_

   %% The Model implements the StorageInterface (Inheritance)
   Model~BusinessType~ --|> StorageInterface : Is_a

   %% The Model owns the actual business data
   Model~BusinessType~ *-- "1" BusinessType : data_

   %% Business classes are independent (No inheritance)
   %% They are used by the Model template
   Model~BusinessType~ ..> User : instantiated with
   Model~BusinessType~ ..> NetworkConfig : instantiated with

   %% Client interacts with the Wrapper
   Client ..> SerializableEntity : archive (vector of SerializableEntity)
```

### Design Note:
This diagram illustrates the "External Polymorphism" or "Type Erasure" 
architecture. The 'SerializableEntity' acts as a value-based wrapper that 
hides the complex inheritance tree from the client. The 'StorageInterface' and 
'Model' classes are internal details that bridge the gap between 
dynamic polymorphism (virtual functions) and static polymorphism 
(templates). This allows 'User' and 'NetworkConfig' to remain 100% 
independent and non-intrusive, as they do not need to inherit from 
any base class to be stored and processed by the system.

**Author:** Mario Galindo Queralt, Ph.D.
