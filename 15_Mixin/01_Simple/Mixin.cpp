/**
 * ============================================================================
 * File: Mixin.cpp (Simple Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates "Horizontal Composition" through Mixin 
 * Inheritance. Using Variadic Templates, we create a generic 'Entity' 
 * that can inherit multiple capabilities (Mixins) at the same time.
 * 
 * Unlike the Decorator pattern (which wraps objects), the Mixin pattern 
 * builds a single, flat object containing all the desired features.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <utility> // std::move

//---------------------------------------------------- Features (Mixin Parts):

//---------------------------------- Laser:
class Laser
{
private:
   int intensity_{2};

public:
   void laser_fire()
   {
      std::cout << "\tLaser fire intensity " << intensity_ << "\n";
      if(intensity_ > 2) --intensity_;
   }

   void laser_set_intensity(int i) 
   {
      if(i < 2) i = 2; 
      intensity_ = i;
   }
};

//----------------------------------- Walk:
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
};

//------------------------------------ Gun:
class Gun
{
private:
   int bullets_{0};

public:
   void gun_fire()
   {
      if(bullets_ > 0)
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
};

//------------------------------------ Fly:
class Fly
{
private:
   int fuel_{0};

public:
   void fly()
   {
      if(fuel_ > 0)
      {
         std::cout << "\tFlying\n";
         --fuel_;
      }
      else
      {
         std::cout << "\tNo fuel to fly\n";
      }
   }

   void load_fuel(int f) 
   {
      fuel_ = f;
   }
};

//--------------------------------------------------------------------- Entity:

template<class ... Mixins> // Inheriting from a pack of templates
class Entity : public Mixins...
{
private:
   std::string name_;

public:
   explicit Entity(std::string n) : name_{std::move(n)} { }

   void print_name() const 
   {
      std::cout << "\n" << name_ << ":\n";
   }
};

//------------------------------------------------------------ Mixin Entities:

// Using aliases to define new types by combining mixins
using Dragon = Entity<Fly, Laser>;

Dragon createDragon(std::string name, int fuel, int intensity)
{
   Dragon d{std::move(name)};
   d.load_fuel(fuel);
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

using Airplane = Entity<Fly, Laser, Gun>;

Airplane createAirplane(std::string name, int fuel, int intensity, int bullets)
{
   Airplane a{std::move(name)};
   a.load_fuel(fuel);
   a.laser_set_intensity(intensity);
   a.gun_set_bullets(bullets);
   return a;
}

//------------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MIXIN PATTERN SIMULATION (HORIZONTAL COMPOSITION) ===\n";

   //---------------------------- Dragon
   Dragon dragon = createDragon("Dragon", 2, 4);
   dragon.print_name();
   dragon.fly();
   dragon.laser_fire();
   dragon.fly();
   dragon.fly();
   std::cout << "\tLoading fuel 1\n";
   dragon.load_fuel(1);
   dragon.fly();
   dragon.fly();
   dragon.laser_fire();
  
   //----------------------------- Elephant
   Elephant elephant = createElephant("Elephant", 3, 2);
   elephant.print_name();
   elephant.walk();
   elephant.gun_fire();
   elephant.gun_fire();
   elephant.gun_fire();
  
   //----------------------------- Airplane
   Airplane airplane = createAirplane("Airplane", 2, 3, 2);
   airplane.print_name();
   airplane.fly();
   airplane.laser_fire();
   airplane.gun_fire();
   airplane.gun_fire();
   airplane.fly();
   airplane.laser_fire();
   airplane.gun_fire();
   airplane.fly();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
