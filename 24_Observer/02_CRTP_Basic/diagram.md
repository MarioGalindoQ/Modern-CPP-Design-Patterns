# Observer Pattern (CRTP Basic Version)

```mermaid
classDiagram
   class Observer~ConcreteObservable~ {
      <<interface>>
      +update(ConcreteObservable&)*
   }

   class Observable~ConcreteObservable~ {
      -set~Observer~ConcreteObservable~*~ observers_
      +attach(Observer~ConcreteObservable~&)
      +detach(Observer~ConcreteObservable~&)
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

   %% Inheritance (Is_a) via CRTP
   Observable~Number~ <|-- Number
   Observer~Number~ <|.. DivObserver
   Observer~Number~ <|.. ModObserver

   %% Composition (Has_a)
   Observable~ConcreteObservable~ *-- "n" Observer~ConcreteObservable~ : observers_

   %% Dependency
   Client ..> Number
   Client ..> Observer~Number~
```

### Design Note:
This implementation leverages the Curiously Recurring Template Pattern
(CRTP). By passing 'Number' as a template argument to the base classes, the
'Observer' interface becomes type-aware at compile-time. The 'update(Number&)'
method provides direct access to the concrete observable's state, improving both
performance (by allowing inlining) and type safety (by eliminating raw pointer
casts).
