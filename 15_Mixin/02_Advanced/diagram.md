# Mixin Pattern (Advanced)

```mermaid
classDiagram
%%---------------------------------------------------------------------- Main:
   class Client {
      +main()
   }

%%---------------------------------------------------- Features (Mixin Parts):
   class Laser {
      +laser_fire()
      +laser_set_intensity(int i)
      +print()
   }
   class Walk {
      +walk()
      +walk_set_speed(int s)
      +print()
   }
   class Gun {
      +gun_fire()
      +gun_set_bullets(int b)
      +print()
   }
   class Tank {
      +tank_load_fuel()
      +tank_get_fuel()
      +print()
   }
   class Fly {
      +fly_set_altitude_speed(int altitude, int speed)
      +fly(auto& entity)
      +print()
   }

%%--------------------------------------------------------------------- Entity:
   class Basic_Entity {
      -string name_
      +print_name()
   }
   class Entity~Mixins~ {
      +Entity~Mixins~(std::string name)
      +visitFeatures(Visitor)
   }

%%------------------------------------------------------------ Mixin Entities:
   class Dragon {
      Dragon~Entity~Basic_Entity Fly Tank Laser~~
      +fly()
   }
   class Elephant {
      Elephant~Entity~Basic_Entity Walk Gun~~
   }
   class Airplane {
      Airplane~Entity~Basic_Entity Fly Tank Laser Gun~~
      +fly()
   }
   class PrintVisitor {
      +operator()(Mixins)
   }

%%----------------------------------------------------------- UML Composition:
   %% Dependencies
   Client ..> Entity~Mixins~

   %% Multiple Inheritance (Horizontal Composition)
   Basic_Entity <|-- Entity~Mixins~
   Walk <|-- Entity~Mixins~
   Fly <|-- Entity~Mixins~
   Tank <|-- Entity~Mixins~
   Laser <|-- Entity~Mixins~
   Gun <|-- Entity~Mixins~

   %% Specific Entities
   Entity~Mixins~ <|-- Dragon
   Entity~Mixins~ <|-- Elephant
   Entity~Mixins~ <|-- Airplane

   %% Dependencies
   Fly ..> Tank : requires
   PrintVisitor ..> Entity~Mixins~ : visits
```

### Design Note:
In this pattern, the 'Entity' class inherits directly from the variadic pack of
'Mixins'. This is 'Static Composition' through Multiple Inheritance. Note that
'Fly' has a dependency on 'Tank', which is resolved at compile-time when both
are mixed into the same Entity.

**Author:** Mario Galindo Queralt, Ph.D.
