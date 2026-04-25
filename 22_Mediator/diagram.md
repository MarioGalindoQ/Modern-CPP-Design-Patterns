# Mediator Pattern

```mermaid
classDiagram
   class Mediator {
      <<interface>>
      +changed(Colleague*)*
   }

   class Colleague {
      <<abstract>>
      #Mediator* mediator_*
      #notify()*
      +setVal(double)
      +setVal(int)
   }

   class Double {
      -double val_
      +setVal(double)
      +getVal() double
   }

   class Integer {
      -int val_
      +setVal(int)
      +getVal() int
   }

   class Expression {
      -Double* a_
      -Double* b_
      -Integer* c_
      -Integer* d_
      -bool doInv_
      -bool doMul_
      -bool doAd1_
      -bool doAd2_
      -double invVal_
      -double mulVal_
      -double ad1Val_
      -double ad2Val_
      +setA(Double*)
      +setB(Double*)
      +setC(Integer*)
      +setD(Integer*)
      +evaluate() double
      +changed(Colleague*)
   }

   class Client {
      +main()
   }

   %% Notify
   Colleague ..> Expression : notify(changed())

   %% Inheritance (Implements)
   Mediator <|.. Expression
   Colleague <|.. Double
   Colleague <|.. Integer

   %% Composition (Has_a)
   Expression o-- "n" Colleague

   %% Composition (Has a)
   Client *-- Mediator
   Client ..> Colleague : setVal()
```

### Design Note:
In this implementation, the 'Expression' (Mediator) acts as a central coordinator
for the calculation (1/a) * (b+c) + d. Instead of Colleagues interacting with
each other, they notify the Mediator when their values change. The Mediator then
decides which parts of the calculation are "dirty" and need to be recomputed,
effectively managing an internal cache of the equation's steps.

**Author:** Mario Galindo Queralt, Ph.D.
