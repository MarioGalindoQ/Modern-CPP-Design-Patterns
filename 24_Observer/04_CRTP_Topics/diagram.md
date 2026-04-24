# Observer Pattern (CRTP with Topics)

```mermaid
classDiagram
   class Topic {
      <<enumeration>>
      Value
      Color
      END
   }

   class Observer~ConcreteObservable, TopicType~ {
      <<interface>>
      +update(ConcreteObservable&, Topic)*
   }

   class Observable~ConcreteObservable~ {
      -vector~set~void*~~ observers_
      +attach(T& obs)
      +detach(T& obs)
      #notify(Topic)
   }

   class NumberObservable {
      -int value_
      -char color_
      +setValue(int)
      +setColor(char)
   }

   class ValueColorObserver {
      +update(NumberObservable&, Topic)
   }

   class ValueObserver {
      +update(NumberObservable&, Topic)
   }

   class ColorObserver {
      +update(NumberObservable&, Topic)
   }

   class Client {
      +main()
   }

   %% Inheritance (Is_a)
   Observable~NumberObservable~ <|-- NumberObservable
   
   %% Multiple Inheritance for observers that listen to multiple topics
   Observer~NumberObservable, Value~ <|.. ValueColorObserver
   Observer~NumberObservable, Color~ <|.. ValueColorObserver
   
   Observer~NumberObservable, Value~ <|.. ValueObserver
   Observer~NumberObservable, Color~ <|.. ColorObserver

   %% Composition (Has_a) - The Observable manages n Observers per topic
   Observable~ConcreteObservable~ *-- "n" Observer~ConcreteObservable, TopicType~ : categorized by Topic

   %% Client interactions
   Client ..> NumberObservable
   Client ..> ValueColorObserver
```

### Design Note:
This implementation represents a high-performance Event Dispatcher. By using
CRTP combined with Topics, we achieve two goals:
1. Static Type Safety: The compiler ensures observers only subscribe to topics
they can actually handle.
2. Granular Notifications: The 'Observable' only notifies the specific subset of
observers interested in a particular change (e.g., notifying 'Color' subscribers
without disturbing 'Value' subscribers).
The 'ValueColorObserver' demonstrates how a single class can satisfy multiple
'Observer' interfaces to listen to different events simultaneously.
