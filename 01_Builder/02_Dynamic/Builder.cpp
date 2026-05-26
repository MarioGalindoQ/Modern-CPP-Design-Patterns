/**
 * ============================================================================
 * File: Builder.cpp (Dynamic Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Builder pattern in a dynamic context. 
 * The main goal is to separate the construction of a complex object (Car) 
 * from its internal representation. 
 * 
 * --- DYNAMIC ALLOCATION & SMART POINTERS:
 * In this version, the Builder creates the final object in the Heap and 
 * returns a 'std::unique_ptr<Car>'. This is the preferred approach when:
 * 1. The object is too large for the stack.
 * 2. The object contains polymorphic components (e.g., different types 
 *    of Engines or Wheels) that require pointer stability.
 * 3. The object's lifetime needs to be managed across different scopes.
 * 
 * --- FLUENT INTERFACE:
 * The Builder uses method chaining (setters returning *this) to allow 
 * a readable and expressive construction process in a single statement.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// -------------------------- Components of a Car:
class Engine
{
private:
   int power_;
public:
   explicit Engine(int power) : power_{power} { }
   int get_power() const { return power_; }
};

class Wheel {/*...*/};
class StandardWheel  : public Wheel {/*...*/};
class HeavyDutyWheel : public Wheel {/*...*/};

//--------------------------------------- The Car:
class Car
{
public:
   enum class Type {Family, Truck, Sport};

   // --- THE RULE OF SEVEN (MGQ Mnemonic System) ---
   Car()                          = delete;  // 1 DC: No default car to force the use of the Builder
   Car(const Car&)                = delete;  // 2 CC: Not possible because unique_ptrs can't be copied
   Car(Car&&) noexcept            = default; // 3 MC: Possible because unique_ptrs can be moved
   Car& operator=(const Car&)     = delete;  // 4 CA: Not possible because unique_ptrs can't be copied
   Car& operator=(Car&&) noexcept = default; // 5 MA: Possible because unique_ptrs can be moved
   ~Car()                         = default; // 6 De: Destructor

private:
   using Engine_ptr    = std::unique_ptr<Engine>; // These are necessary because Engine and
   using Wheel_ptr     = std::unique_ptr<Wheel>;  // Wheel classes could be polymorphic.
   using Wheels_vector = std::vector<Wheel_ptr>;

   float         weight_;
   float         length_;
   float         width_;
   int           doorCount_;
   std::string   color_;
   Type          type_;
   Engine_ptr    engine_;
   Wheels_vector wheels_;

   // 7 PC: Very complicated particular constructor (intentionally private):
   Car(float weight, float length, float width, int doorCount, std::string color,
       Type type, Engine_ptr engine, Wheels_vector wheels)
      : weight_{weight}, length_{length}, width_{width}, doorCount_{doorCount},
        color_{std::move(color)}, type_{type}, engine_{std::move(engine)},
        wheels_{std::move(wheels)} { }

   friend class Builder; // Only local Builder class can build Cars

public:
   void print() const
   {
      std::cout << "Car: weight = " << weight_ << ", length = " << length_ << ", width = " << width_
                << ", doorCount = " << doorCount_ << ", wheels = " << wheels_.size() << ", color = " << color_
                << ", enginePower = " << engine_->get_power() << ", type = "
                << (type_==Type::Family ? "Family" : type_==Type::Truck ? "Truck" : "Sport") << std::endl;
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

      template<class T, typename=std::enable_if_t<std::is_base_of_v<Wheel, T>>>
      Wheels_vector buildWheels() const
      {
         Wheels_vector wheels;
         wheels.reserve(wheelCount_);
         for(auto i{0}; i<wheelCount_; ++i) wheels.emplace_back(std::make_unique<T>());
         return wheels;
      }

   public:
      Builder& setWeight     (float we)       noexcept {weight_     = we;            return *this;}
      Builder& setLength     (float le)       noexcept {length_     = le;            return *this;}
      Builder& setWidth      (float wi)       noexcept {width_      = wi;            return *this;}
      Builder& setDoorCount  (int dc)         noexcept {doorCount_  = dc;            return *this;}
      Builder& setColor      (std::string co) noexcept {color_      = std::move(co); return *this;}
      Builder& setPower      (int po)         noexcept {power_      = po;            return *this;}
      Builder& setWheelCount (int wc)         noexcept {wheelCount_ = wc;            return *this;}

      std::unique_ptr<Car> build() const
      {
         Type type;
         if(power_ > 400) type = (wheelCount_ > 4) ? Type::Truck : Type::Sport;
         else             type = Type::Family;
 
         Wheels_vector wheels;
         if(wheelCount_>4) wheels = buildWheels<HeavyDutyWheel>();
         else              wheels = buildWheels<StandardWheel>();

         return std::unique_ptr<Car>(new Car{weight_, length_, width_, doorCount_, std::move(color_),
                                             type, std::make_unique<Engine>(power_),
                                             std::move(wheels)});
      } // Build

      // Conversion Operator
      operator std::unique_ptr<Car>() const { return build(); }

   }; // Builder
}; // Car

//------------------------------------------------------------------------------- Main
int main()
{
   // Example 1: Default-ish construction
   std::unique_ptr<Car> car = Car::Builder{}; // calling .build() is not necessary due to implicit conversion.
   car->print();

   // Example 2: Sport configuration (Fluent interface)
   car = Car::Builder{}.setColor("white")
                       .setDoorCount(3)
                       .setWidth(1.6f)
                       .setLength(4.0f)
                       .setWheelCount(4)
                       .setPower(550)
                       .build(); // anyway, .build() can be called!
   car->print();

   // Example 3: Truck configuration (Fluent interface)
   car = Car::Builder{}.setLength(5.5f)
                       .setWidth(2.6f)
                       .setWeight(3.1f)
                       .setDoorCount(2)
                       .setWheelCount(6)
                       .setPower(900);
   car->print();
}

//============================================================================================= END
