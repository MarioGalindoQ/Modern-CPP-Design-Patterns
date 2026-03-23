#include <string>
#include <memory>
#include <vector>
#include <iostream>

// -------------------------- Components of a Car:
class Engine
{
private:
   int power_;
public:
   explicit Engine(int power) : power_{power} { }
};

class Wheel {/*...*/};

class StandardWheel  : public Wheel {/*...*/};

class HeavyDutyWheel : public Wheel {/*...*/};

//--------------------------------------- The Car:
class Car
{
public:
   enum class Type {Family, Truck, Sport};

private:
   using Wheel_ptr     = std::unique_ptr<Wheel>; // This is necessary because
   using Wheels_vector = std::vector<Wheel_ptr>; // Wheel class is polymorphic

   float         weight_;
   float         width_;
   int           doorCount_;
   std::string   color_;
   Type          type_;
   Engine        engine_;
   Wheels_vector wheels_;

   // Very complicated constructor (intentionally private):
   Car(float weight,  float width,  int doorCount,  std::string color, Type type,
       Engine engine,  Wheels_vector wheels)
      : weight_{weight},  width_{width},  doorCount_{doorCount},  color_{std::move(color)},
        type_{type},  engine_{std::move(engine)},  wheels_{std::move(wheels)} { }

   friend class Builder; // Only Builder can build Cars

public:
   void print()
   {
      std::cout << "Car: weight = " << weight_    << ", width = " << width_
                << ", doorCount = " << doorCount_ << ", color = " << color_
                << std::endl;
   }

   class Builder final // Use Builder to manage such complicated constructor.
   {
   private:
      float       weight_{2}; // default values
      float       width_{4};
      int         doorCount_{4};
      std::string color_{"black"};
      int         power_{100};
      int         wheelCount_{4};

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
      Builder& setWidth      (float wi)       noexcept {width_      = wi;            return *this;}
      Builder& setDoorCount  (int dc)         noexcept {doorCount_  = dc;            return *this;}
      Builder& setColor      (std::string co) noexcept {color_      = std::move(co); return *this;}
      Builder& setPower      (int po)         noexcept {power_      = po;            return *this;}
      Builder& setWheelCount (int wc)         noexcept {wheelCount_ = wc;            return *this;}

      operator Car() // This operator converts a Builder into a Car with intelligence
      {
         Type type;
         if(power_ > 500) type = (wheelCount_>4) ? Type::Truck : Type::Sport;
         else             type = Type::Family;
      
         Wheels_vector wheels;
         if(wheelCount_>4) wheels = buildWheels<HeavyDutyWheel>();
         else              wheels = buildWheels<StandardWheel>();
      
         return Car{weight_, width_, doorCount_, std::move(color_),
               type, Engine{power_}, std::move(wheels)};
      }

      Car build()
      {
         return Car(*this);
      }
   }; // Builder
}; // Car

int main()
{
   Car car1 = Car::Builder{}.setColor("blue")
                            .setDoorCount(2)
                            .setWidth(3.5f)
                            .setWheelCount(4)
                            .setPower(800); //.build(); // is not necessary here
   car1.print();

   auto car2 = Car::Builder{}.setWheelCount(6)
                             .setWidth(4.1f)
                             .setPower(1200)
                             .setColor("black")
                             .setDoorCount(3)
                             .setWeight(3).build(); // But here build() is necessary
   car2.print();
}

//============================================================================================= END
