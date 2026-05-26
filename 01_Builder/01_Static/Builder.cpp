/**
 * ============================================================================
 * File: Builder_Stack.cpp (Pure Stack Version)
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This version is optimized for maximum performance by avoiding the Heap
 * entirely. All components, including the collection of wheels, are
 * allocated on the Stack.
 *
 * --- STACK ALLOCATION & EFFICIENCY:
 * Unlike the dynamic version, this implementation returns the final object
 * by value. This avoids the overhead of heap allocation (new/delete) and
 * is ideal for objects whose lifetime is tied to the local scope.
 *
 * --- STACK POLYMORPHISM:
 * Instead of 'std::unique_ptr<Wheel>', we use 'std::variant'. This allows
 * different wheel types to coexist in a 'std::array' without dynamic
 * allocation or pointer indirection.
 *
 * --- ADVANCED C++ FEATURES:
 * 1. Fluent Interface: Setter methods return a reference to the builder 
 *    (*this), allowing for elegant method chaining.
 * 2. Conversion Operator: The implementation includes 'operator Car()', 
 *    enabling implicit conversion from the Builder to the Product.
 * 3. Explicit Build: A '.build()' method is provided for cases where 
 *    explicit object finalization is preferred for clarity.
 * 4. Stack Polymorphism: Uses 'std::variant' and 'std::array' to handle 
 *    different wheel types without heap-based pointers.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <array>
#include <variant>

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

   class Builder final // Use Builder to manage such complicated constructor.
   {
   private:
      float       weight_     {1.3f}; // default values
      float       length_     {2.2f};
      float       width_      {1.8f};
      int         doorCount_  {4};
      std::string color_      {"black"};
      int         power_      {100};
      int         wheelCount_ {4};

   public:
      Builder& setWeight     (float we)       noexcept {weight_     = we;                            return *this;}
      Builder& setLength     (float le)       noexcept {length_     = le;                            return *this;}
      Builder& setWidth      (float wi)       noexcept {width_      = wi;                            return *this;}
      Builder& setDoorCount  (int dc)         noexcept {doorCount_  = dc;                            return *this;}
      Builder& setColor      (std::string co) noexcept {color_      = std::move(co);                 return *this;}
      Builder& setPower      (int po)         noexcept {power_      = po;                            return *this;}
      Builder& setWheelCount (int wc)         noexcept {wheelCount_ = (wc>MAX_WHEELS)?MAX_WHEELS:wc; return *this;}

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
   Car car2 = car; // Calls 2 CC (Copy Constructor)
   std::cout << "Car 2 (Copy of Car):\n";
   car2.print();
}

//================================================================================ END
