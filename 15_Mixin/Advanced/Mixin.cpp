/**
 * ============================================================================
 * File: Mixin.cpp (Advanced Version)
 * 
 * --- DESIGN OVERVIEW:
 * This advanced implementation demonstrates two powerful C++ techniques:
 * 1. Mixin Inter-dependency: The 'Fly' mixin requires the 'Tank' mixin 
 *    to function, showing how components can interact within the same entity.
 * 2. Static Reflection/Visitation: Using Fold Expressions (C++17) to 
 *    iterate over all inherited base classes (Mixins) at compile-time.
 * 
 * This approach provides a high-performance alternative to the traditional 
 * Visitor pattern, avoiding virtual tables and dynamic dispatch.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <utility>

//--------------------------------------------------------- Features (Mixins):

class Laser
{
private:
   int intensity_{2};

public:
   void laser_fire()
   {
      std::cout << "\tLaser fire intensity " << intensity_ << "\n";
      if (intensity_ > 2) --intensity_;
   }

   void laser_set_intensity(int i) 
   {
      if (i < 2) i = 2; 
      intensity_ = i;
   }

   void print() const 
   {
      std::cout << "  - Laser with intensity " << intensity_ << "\n";
   }
};

class Walk
{
private:
   int speed_{0};

public:
   void walk() const
   {
      std::cout << "\tWalk speed " << speed_ << "\n";
   }

   void walk_set_speed(int s) 
   {
      speed_ = s;
   }

   void print() const 
   {
      std::cout << "  - Walk with speed " << speed_ << "\n";
   }
};

class Gun
{
private:
   int bullets_{0};

public:
   void gun_fire()
   {
      if (bullets_ > 0) 
      {
         std::cout << "\tGun fire\n"; 
         --bullets_;
      }
      else
      {
         std::cout << "\tGun no more bullets\n";
      }
   }

   void gun_set_bullets(int b) 
   {
      bullets_ = b;
   }

   void print() const 
   {
      std::cout << "  - Gun with " << bullets_ << " bullets\n";
   }
};

class Tank
{
private:
   int fuel_{0};

public:
   void tank_load_fuel(int f) 
   {
      fuel_ = f;
   }

   bool tank_get_fuel(int quantity)
   {
      if (fuel_ < quantity) return false;
      fuel_ -= quantity;
      return true;
   }

   void print() const 
   {
      std::cout << "  - Tank with " << fuel_ << " of fuel\n";
   }
};

class Fly
{
private:
   int altitude_{300};
   int speed_{40};

public:
   // Alternative 1: Rigid dependency (Only Tank can be used)
   // void fly(Tank& tank)

   // Alternative 2: Generic template (The Entity type must be explicitly defined)
   // Any class that provides 'tank_get_fuel' is accepted.
   // The Entity type is well know inside fly() method.
   // template<class Entity>
   // void fly(Entity& entity)

   // Alternative 3: Abbreviated Function Template (Modern C++20)
   // The Entity type is deduced at compile-time.
   // Any class that provides 'tank_get_fuel' is accepted, achieving complete
   // decoupling without requiring inheritance or explicit template parameters.
   // The Entity type isn't know inside fly() method.
   void fly(auto& entity)
   {
      if (entity.tank_get_fuel(1)) std::cout << "\tFlying at " << altitude_ << " ft\n";
      else                         std::cout << "\tNo fuel to Fly\n";
   }

   void print() const 
   {
      std::cout << "  - Fly capability: Alt " << altitude_ << " / Speed " << speed_ << "\n";
   }
};

//------------------------------------------------------------- Generic Entity:

class Basic_Entity
{
private:
   std::string name_;

public:
   explicit Basic_Entity(std::string n) : name_{std::move(n)} { }

   void print_name() const 
   {
      std::cout << "\n" << name_ << ":\n";
   }
};

// Variadic Template Entity inheriting from all Mixins
template<class ... Mixins>
class Entity : public Basic_Entity, public Mixins...
{
public:
   explicit Entity(std::string name) : Basic_Entity{std::move(name)}, Mixins()... { }

   // Advanced Visitor: Uses Fold Expressions to apply a function to all bases
   template<typename Visitor>
   void visitFeatures(Visitor visitor)
   {
      // static_cast unpacks the 'this' pointer into each specific base class
      visitor(static_cast<Mixins&>(*this)...);
   }
};

//--------------------------------------------------------- Mixin Visitor:

struct PrintVisitor
{
   template<typename... Mixins>
   void operator()(Mixins&... mixins) const
   {
      // C++17 Fold Expression: call print() on every mixin in the pack
      (mixins.print(), ...);
   }
};

//--------------------------------------------------------- Specific Entities:

// Extension for Dragon to use fly() without arguments:
// using Dragon = public Entity<Fly, Tank, Laser>
class Dragon : public Entity<Fly, Tank, Laser>
{
public:
   using Entity::Entity; // Get Entity constructor
   
   void fly() 
   {
      Fly::fly(*this);
   }
};

Dragon createDragon(std::string name, int fuel, int intensity)
{
   Dragon d{std::move(name)};
   d.tank_load_fuel(fuel);
   d.laser_set_intensity(intensity);
   return d;
}

using Elephant = Entity<Walk, Gun>;

Elephant createElephant(std::string name, int speed, int bullets)
{
   Elephant e{std::move(name)};
   e.walk_set_speed(speed);
   e.gun_set_bullets(bullets);
   return e;
}

// Extension for Airplane to use fly() without arguments:
// using Airplane = Entity<Fly, Tank, Laser, Gun>
class Airplane : public Entity<Fly, Tank, Laser, Gun>
{
public:
   using Entity::Entity; // Get Entity constructor

   void fly() 
   {
      Fly::fly(*this);
   }
};

Airplane createAirplane(std::string name, int fuel, int intensity, int bullets)
{
   Airplane a{std::move(name)};
   a.tank_load_fuel(fuel);
   a.laser_set_intensity(intensity);
   a.gun_set_bullets(bullets);
   return a;
}

//------------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MIXIN PATTERN (ADVANCED VISITOR & DEPENDENCIES) ===\n";

   //------------------------------- Dragon
   Dragon dragon = createDragon("Ancient Dragon", 2, 4);
   dragon.print_name();
   dragon.visitFeatures(PrintVisitor{});

   std::cout << "  + Testing dragon:" << std::endl;
   dragon.fly();
   dragon.laser_fire();
   dragon.fly();
   dragon.fly(); // Should fail (no fuel)
   
   std::cout << "\tLoading fuel 1\n";
   dragon.tank_load_fuel(1);
   
   dragon.fly(); // Now it should work
   dragon.laser_fire();
  
   //----------------------------- Elephant
   Elephant elephant = createElephant("War Elephant", 3, 2);
   elephant.print_name();
   elephant.visitFeatures(PrintVisitor{});
   
   std::cout << "  + Testing elephant:" << std::endl;
   elephant.walk();
   elephant.gun_fire();
   elephant.gun_fire();
   elephant.gun_fire();
  
   //----------------------------- Airplane
   Airplane airplane = createAirplane("Combat Jet", 2, 5, 3);
   airplane.print_name();
   airplane.visitFeatures(PrintVisitor{});
   
   std::cout << "  + Testing ariplane:" << std::endl;
   airplane.fly();
   airplane.laser_fire();
   airplane.gun_fire();
   airplane.gun_fire();
   airplane.fly();
   airplane.fly(); // Should fail

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
