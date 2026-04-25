# Mixin Pattern (Simple)

```mermaid
classDiagram
%%---------------------------------------------------------------------- Main:
   class Client {
      +main()
   }

%%---------------------------------------------------- Features (Mixin Parts):
   class Laser {
      +laser_fire()
      +laser_set_intensity(int)
   }

   class Walk {
      +walk()
      +walk_set_speed(int)
   }

   class Gun {
      +gun_fire()
      +gun_set_bullets(int)
   }

   class Fly {
      +fly()
      +fly_load_fuel(int)
   }

%%--------------------------------------------------------------------- Entity:
   class Entity~Mixins~ {
      +string name_
      +Entity(string)
      +print_name() : string
   }

%%------------------------------------------------------------ Mixin Entities:
   %% Mixin entities defined by type aliases in the code
   class Dragon{Dragon~Entity~Fly Laser~~}
   class Elephant {Elephant~Entity~Walk Gun~~}
   class Airplane {Airplane~Entity~Fly Laser Gun~~}

%%----------------------------------------------------------- UML Composition:
   %% The client has an Entity<Mixins>>
   Client ..> Entity~Mixins~

   %% Multiple Inheritance (Horizontal Composition)
   %% Inheritance arrows do not use multiplicity numbers
   Laser <|-- Entity~Mixins~
   Walk <|-- Entity~Mixins~
   Gun <|-- Entity~Mixins~
   Fly <|-- Entity~Mixins~

   %% Relationships for specific types
   Entity~Mixins~ <|-- Dragon
   Entity~Mixins~ <|-- Elephant
   Entity~Mixins~ <|-- Airplane
```

### Design Note:
In the Simple Mixin version, the 'Entity' class provides a way to combine
independent features (Mixins) at compile-time. The final objects (Dragon,
Elephant, Airplane) are flat structures that contain all the methods and data
members of their respective base Mixins simultaneously.

**Author:** Mario Galindo Queralt, Ph.D.
