/**
 * ============================================================================
 * File: Builder_Deducing.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This version evolves the Static Builder by using the C++23 feature: 
 * "Deducing This" (Explicit Object Parameters) applied to a Multi-Mixin 
 * architecture.
 *
 * --- THE ARCHITECTURAL ADVANTAGE (MULTI-MIXIN COMPOSITION):
 * Traditionally, splitting a Builder into multiple specialized classes 
 * (e.g., Physical, Mechanical, and Aesthetic) created a "Return Type Problem". 
 * If a method in a base class returns a reference to itself, the Fluent 
 * Interface breaks because the compiler "forgets" the derived Builder type.
 *
 * C++23 'this auto&& self' solves this elegantly. By using the explicit 
 * object parameter, methods in any parent class automatically deduce and 
 * return the outermost derived type (the Builder). This allows us to:
 * 1. Modularize State: Each Mixin class manages its own domain of properties.
 * 2. Maintain Encapsulation: Using 'class' with 'protected' data ensures 
 *    that properties are only modified through the public Fluent API.
 * 3. Zero Complexity: We avoid the "Template Hell" of CRTP while keeping 
 *    the performance of static polymorphism.
 *
 * --- TECHNICAL MECHANICS:
 * 1. Deducing This: 'auto&& setter(this auto&& self, ...)' captures the 
 *    actual Builder instance, even when called from a base class.
 * 2. Value Category Preservation: Using 'auto&&' and 'std::forward' ensures 
 *    that the Builder works perfectly with both lvalues and temporaries.
 * 3. Multi-Inheritance: The Builder acts as a pure orchestrator, inheriting 
 *    all state and setters from its specialized parents.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <array>
#include <variant>
#include <utility> // Required for std::forward

// -------------------------- Components of a Car:
class Engine
{
private:
   int power_;
public:
   explicit Engine(int power) : power_{power} { }
   int get_power() const { return power_; }
};

// No need for a base class with std::variant
class StandardWheel  { /*...*/ };
class HeavyDutyWheel { /*...*/ };

// Variant acts as a stack-based polymorphic container
using WheelVariant = std::variant<StandardWheel, HeavyDutyWheel>;

//--------------------------------------- The Car:
class Car
{
public:
   enum class Type {Family, Truck, Sport};

   // --- THE RULE OF SEVEN (MGQ Mnemonic System) ---
   Car()                          = delete;  // 1 DC: No default car to force the use of the Builder
   Car(const Car&)                = default; // 2 CC: Possible on stack
   Car(Car&&) noexcept            = default; // 3 MC: Efficient transfer
   Car& operator=(const Car&)     = default; // 4 CA: Possible on stack
   Car& operator=(Car&&) noexcept = default; // 5 MA: Efficient re-assignment
   ~Car()                         = default; // 6 De: Destructor

private:
   // Static constraint: Max wheels on the stack
   static constexpr int MAX_WHEELS = 8;

   float         weight_;
   float         length_;
   float         width_;
   int           doorCount_;
   std::string   color_;
   Type          type_;
   Engine        engine_;
   int           activeWheels_;

   // STACK STORAGE: Fixed size array of variants for wheels
   using Wheels_array = std::array<WheelVariant, MAX_WHEELS>;
   Wheels_array wheels_;

   // 7 PC: Very complicated particular constructor (intentionally private):
   Car(float weight, float length, float width, int doorCount, std::string color,
       Type type, Engine engine, int activeWheels, Wheels_array wheels)
      : weight_{weight}, length_{length}, width_{width}, doorCount_{doorCount},
        color_{std::move(color)}, type_{type}, engine_{engine}, 
        activeWheels_{activeWheels}, wheels_{wheels} { }

   friend class Builder; // Only local Builder class can build Cars

public:
   void print() const
   {
      std::cout << "Car: weight = " << weight_ << ", length = " << length_ << ", width = " << width_
                << ", doorCount = " << doorCount_ << ", wheels = " << activeWheels_ << ", color = "
                << color_ << ", enginePower = " << engine_.get_power() << ", type = "
                << (type_==Type::Family ? "Family" : type_==Type::Truck ? "Truck" : "Sport")
                << std::endl;
   }

   // --- Hierarchical Builder Mixins using C++23 Deducing This ---

   class PhysicalProps
   {
   protected:
      float weight_ {1.3f};
      float length_ {2.2f};
      float width_  {1.8f};

   public:
      // Using auto&& self handles both lvalues and rvalues (temporaries)
      auto&& setWeight(this auto&& self, float we) noexcept
      {
         self.weight_ = we; 
         return std::forward<decltype(self)>(self);
      }
      auto&& setLength(this auto&& self, float le) noexcept
      {
         self.length_ = le; 
         return std::forward<decltype(self)>(self);
      }
      auto&& setWidth(this auto&& self, float wi) noexcept
      {
         self.width_ = wi; 
         return std::forward<decltype(self)>(self);
      }
   };

   class MechanicalProps
   {
   protected:
      int power_ {100};

   public:
      auto&& setPower(this auto&& self, int po) noexcept
      {
         self.power_ = po;
         return std::forward<decltype(self)>(self);
      }
   };

   class AestheticProps
   {
   protected:
      int         doorCount_  {4};
      std::string color_      {"black"};
      int         wheelCount_ {4};

   public:
      auto&& setDoorCount(this auto&& self, int dc) noexcept
      {
         self.doorCount_ = dc;
         return std::forward<decltype(self)>(self);
      }
      auto&& setColor(this auto&& self, std::string co) noexcept
      {
         self.color_ = std::move(co);
         return std::forward<decltype(self)>(self);
      }
      auto&& setWheelCount(this auto&& self, int wc) noexcept
      {
         self.wheelCount_ = (wc > MAX_WHEELS) ? MAX_WHEELS : wc;
         return std::forward<decltype(self)>(self);
      }
   };

   // Final Builder composed of specialized Mixins
   class Builder final : public PhysicalProps, public MechanicalProps, public AestheticProps
   {
   public:
      Car build() const
      {
         Type type;
         if(power_ > 400) type = (wheelCount_ > 4) ? Type::Truck : Type::Sport;
         else             type = Type::Family;

         Car::Wheels_array wheels;
         for(int i{0}; i < wheelCount_; ++i)
         {
            if(wheelCount_ > 4) wheels[i] = HeavyDutyWheel{};
            else                wheels[i] = StandardWheel{};
         }

         return Car{weight_, length_, width_, doorCount_, std::move(color_),
                    type, Engine(power_), wheelCount_, wheels};
      } // Build

      // Conversion Operator
      operator Car() const { return build(); }

   }; // Builder
}; // Car

//------------------------------------------------------------------------------- Main
int main()
{
   // Example 1: Default-ish construction
   Car car = Car::Builder{}; // calling .build() is not necessary due to implicit conversion.
   car.print();

   // Example 2: Sport configuration (Fluent interface)
   car = Car::Builder{}.setColor("white")
                       .setDoorCount(3)
                       .setWidth(1.6f)
                       .setLength(4.0f)
                       .setWheelCount(4)
                       .setPower(550)
                       .build(); // anyway, .build() can be called!
   car.print();

   // Example 3: Truck configuration (Fluent interface)
   car = Car::Builder{}.setLength(5.5f)
                       .setWidth(2.6f)
                       .setWeight(3.1f)
                       .setDoorCount(2)
                       .setWheelCount(6)
                       .setPower(900);
   car.print();

   // Demonstration of Copying
   Car car2 = car; 
   std::cout << "Car 2 (Copy of Car):\n";
   car2.print();
}

//================================================================================ END
