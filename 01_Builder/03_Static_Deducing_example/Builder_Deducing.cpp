/**
 * ============================================================================
 * File: Builder_Deducing.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This version evolves the Static Builder by using the C++23 feature: 
 * "Deducing This" (Explicit Object Parameters). 
 *
 * --- THE ARCHITECTURAL ADVANTAGE:
 * In Example 01, the Builder was a single flat class. If we wanted to split 
 * the Builder into a hierarchy (e.g., PhysicalBuilder -> CarBuilder), we 
 * would traditionally need CRTP to ensure that base methods return a 
 * reference to the derived class.
 *
 * With C++23 'this auto&& self', the base class methods automatically deduce 
 * and return the correct derived type, enabling a clean, hierarchical 
 * Fluent Interface without template complexity.
 *
 * --- TECHNICAL MECHANICS:
 * 1. Deducing This: 'auto&& setWeight(this auto&& self, ...)' captures the 
 *    actual type of the object at the call site, along with its value 
 *    category (lvalue or rvalue).
 * 2. Hierarchical Fluent Interface: Methods defined in the base class can 
 *    be chained with methods in the derived class seamlessly because 'self'
 *    always refers to the most derived type in the chain.
 * 3. Forwarding: By using 'std::forward', we preserve the value category 
 *    of the builder throughout the entire fluent chain.
 * 4. Stack-Based: Maintains the Zero-Heap philosophy of Example 01.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <array>
#include <variant>
#include <utility> // Required for std::forward

// -------------------------- Components of a Car:
class Engine {
private:
   int power_;
public:
   explicit Engine(int power) : power_{power} { }
   int get_power() const { return power_; }
};

class StandardWheel  { };
class HeavyDutyWheel { };

using WheelVariant = std::variant<StandardWheel, HeavyDutyWheel>;

//--------------------------------------- The Car:
class Car {
public:
   enum class Type {Family, Truck, Sport};

   // --- THE RULE OF SEVEN ---
   Car()                          = delete; 
   Car(const Car&)                = default; 
   Car(Car&&) noexcept            = default; 
   Car& operator=(const Car&)     = default; 
   Car& operator=(Car&&) noexcept = default; 
   ~Car()                         = default; 

private:
   static constexpr int MAX_WHEELS = 8;

   float         weight_;
   float         length_;
   float         width_;
   int           doorCount_;
   std::string   color_;
   Type          type_;
   Engine        engine_;
   int           activeWheels_;

   using Wheels_array = std::array<WheelVariant, MAX_WHEELS>;
   Wheels_array wheels_;

   Car(float weight, float length, float width, int doorCount, std::string color,
       Type type, Engine engine, int activeWheels, Wheels_array wheels)
      : weight_{weight}, length_{length}, width_{width}, doorCount_{doorCount},
        color_{std::move(color)}, type_{type}, engine_{engine}, 
        activeWheels_{activeWheels}, wheels_{wheels} { }

   friend class Builder; 

public:
   void print() const {
      std::cout << "Car: weight = " << weight_ << ", length = " << length_ << ", width = " << width_
                << ", doorCount = " << doorCount_ << ", wheels = " << activeWheels_ << ", color = " << color_
                << ", enginePower = " << engine_.get_power() << ", type = "
                << (type_==Type::Family ? "Family" : type_==Type::Truck ? "Truck" : "Sport") << std::endl;
   }

   // --- Hierarchical Builder using C++23 Deducing This ---

   struct PhysicalProps {
      float weight_ {1.3f};
      float length_ {2.2f};
      float width_  {1.8f};

      // Using auto&& self handles both lvalues and rvalues (temporaries)
      auto&& setWeight(this auto&& self, float we) noexcept { 
         self.weight_ = we; 
         return std::forward<decltype(self)>(self); 
      }
      auto&& setLength(this auto&& self, float le) noexcept { 
         self.length_ = le; 
         return std::forward<decltype(self)>(self); 
      }
      auto&& setWidth(this auto&& self, float wi) noexcept { 
         self.width_ = wi; 
         return std::forward<decltype(self)>(self); 
      }
   };

   class Builder final : public PhysicalProps {
   private:
      int         doorCount_  {4};
      std::string color_      {"black"};
      int         power_      {100};
      int         wheelCount_ {4};

   public:
      auto&& setDoorCount(this auto&& self, int dc) noexcept { 
         self.doorCount_ = dc; 
         return std::forward<decltype(self)>(self); 
      }
      auto&& setColor(this auto&& self, std::string co) noexcept { 
         self.color_ = std::move(co); 
         return std::forward<decltype(self)>(self); 
      }
      auto&& setPower(this auto&& self, int po) noexcept { 
         self.power_ = po; 
         return std::forward<decltype(self)>(self); 
      }
      auto&& setWheelCount(this auto&& self, int wc) noexcept { 
         self.wheelCount_ = (wc > MAX_WHEELS) ? MAX_WHEELS : wc; 
         return std::forward<decltype(self)>(self); 
      }

      Car build() const {
         Type type;
         if(power_ > 400) type = (wheelCount_ > 4) ? Type::Truck : Type::Sport;
         else             type = Type::Family;
      
         Car::Wheels_array wheels;
         for(int i{0}; i < wheelCount_; ++i) {
            if(wheelCount_ > 4) wheels[i] = HeavyDutyWheel{};
            else                wheels[i] = StandardWheel{};
         }
      
         return Car{weight_, length_, width_, doorCount_, std::move(color_),
                    type, Engine(power_), wheelCount_, wheels};
      }

      operator Car() const { return build(); }
   }; 
}; 

//------------------------------------------------------------------------------- Main
int main() {
   // Example 1: Default-ish construction
   Car car = Car::Builder{}; 
   car.print();

   // Example 2: Sport configuration (Fluent interface)
   car = Car::Builder{}.setColor("white")
                       .setDoorCount(3)
                       .setWidth(1.6f)
                       .setLength(4.0f)
                       .setWheelCount(4)
                       .setPower(550)
                       .build(); 
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
