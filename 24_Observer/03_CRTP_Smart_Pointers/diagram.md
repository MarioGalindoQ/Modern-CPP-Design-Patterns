# Observer Pattern (CRTP with Smart Pointers)

```mermaid
classDiagram
   class Observer~T~ {
      <<interface>>
      +update(T&)*
   }

   class Observable~T~ {
      -vector~weak_ptr~ observers_
      +attach(shared_ptr~Observer~)
      +detach(shared_ptr~Observer~)
      #notify()
   }

   class Number {
      -int val_
      +setVal(int)
      +getVal() int
   }

   class DivObserver {
      -int div_
      +update(Number&)
   }

   class ModObserver {
      -int div_
      +update(Number&)
   }

   class Client {
      +main()
   }

   Observable~Number~ <|-- Number
   Observer~Number~ <|.. DivObserver
   Observer~Number~ <|.. ModObserver

   Observable~T~ *-- "n" Observer~T~ : weak_ptr collection

   Client ..> Number
   Client ..> Observer~Number~ : owns shared_ptr
```

### Design Note:
This version introduces memory safety into the CRTP Observer. The Observable
maintains a collection of 'std::weak_ptr' to its observers. This prevents
circular dependencies (leaks) and ensures that if an observer is destroyed by
the Client, the Observable won't attempt to access invalid memory. During
'notify()', each pointer is temporarily locked to verify the observer's
existence before calling 'update()'.

**Author:** Mario Galindo Queralt, Ph.D.
