# Builder Pattern (Modern C++23 Deducing This)

```mermaid
classDiagram
   class Car {
      -float weight_
      -float length_
      -float width_
      -int doorCount_
      -string color_
      -Type type_
      -Engine engine_
      -Wheels_array wheels_
      +print() void
   }

   class Engine {
      -int power_
      +get_power() int
   }

   class PhysicalProps {
      +float weight_
      +float length_
      +float width_
      +setWeight(this auto&& self, float) auto&&
      +setLength(this auto&& self, float) auto&&
      +setWidth(this auto&& self, float) auto&&
   }

   class Builder {
      -int doorCount_
      -string color_
      -int power_
      -int wheelCount_
      +setDoorCount(this auto&& self, int) auto&&
      +setColor(this auto&& self, string) auto&&
      +setPower(this auto&& self, int) auto&&
      +setWheelCount(this auto&& self, int) auto&&
      +build() Car
   }

   %% Hierarchical Builder Relationship
   Builder --|> PhysicalProps

   %% Car Composition
   Car *-- Engine
   Car *-- "8" WheelVariant

   %% Dependencies
   Builder ..> Car : builds
   Client ..> Builder : uses to configure
   Client ..> Car : consumes product
```

### Design Note:
This diagram illustrates the **Modern Hierarchical Builder** enabled by C++23's 
**Deducing This** feature. 

1. **Simplified Hierarchy:** Unlike Example 01 (Flat Builder) or a traditional 
   CRTP implementation, `PhysicalProps` is a standard class (not a template). 
   It can be inherited by any number of specialized Builders.
2. **Explicit Object Parameter:** By using `this auto&& self` in the base class 
   setters, the methods automatically capture and return the derived `Builder` 
   type. This preserves the **Fluent Interface** throughout the entire 
   inheritance chain without "losing" the derived type.
3. **Value Category Awareness:** The use of `auto&&` ensures that the Builder 
   correctly handles both lvalues and temporaries (rvalues), making the 
   chaining robust and efficient.
4. **Stack-Based Architecture:** The design remains faithful to the Zero-Heap 
   philosophy, using `std::variant` and `std::array` for stack-polymorphism 
   within the `Car` product.

**Author:** Mario Galindo Queralt, Ph.D.
