/**
 * ============================================================================
 * File: VariantValueClone.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the "Value Semantics" approach using std::variant
 * as a modern alternative to traditional polymorphism and the Prototype pattern.
 * 
 * --- THE ARCHITECTURAL SHIFT:
 * In the example (32/CRTP/03_PolymorphicClone), we used heap allocation and 
 * a CRTP Mixin to handle cloning. Here, we store objects directly on the stack 
 * (inside the variant). 
 * 
 * --- ADVANTAGES:
 * 1. Automatic Deep Copy: Since std::variant and the concrete classes (Square, 
 *    Circle) have value semantics, cloning the entire container is handled 
 *    automatically by the language.
 * 2. No Pointers: We eliminate memory management concerns and the need for 
 *    smart pointers or explicit 'clone()' methods.
 * 3. Static Dispatch: We use std::visit to invoke behaviors like 'draw()', 
 *    allowing the compiler to optimize the calls without relying on vtables.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <variant>

//---------------------------------------------------- Abstract Base Shape:
class Shape // Base
{
public:
   virtual ~Shape() = default;
   virtual void draw() const = 0;
};

//-------------------------------------------------- Concrete Shape Square:
class Square : public Shape
{
public:
   Square() // Constructor
   {
      std::cout << " [Constructor] Created a new Square\n";
   }

   Square(const Square& other) : Shape(other) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copied a Square\n";
   }

   Square(Square&& other) noexcept : Shape(std::move(other))
   {
      std::cout << " [Move] Moved a Square\n"; // Move constructor
   }

   void draw() const override
   {
      std::cout << " [Drawing] Square at memory address: " << this << "\n";
   }
};

//-------------------------------------------------- Concrete Shape Circle:
class Circle : public Shape
{
public:
   Circle() // Constructor
   {
      std::cout << " [Constructor] Created a new Circle\n";
   }

   Circle(const Circle& other) : Shape(other) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copied a Circle\n";
   }

   Circle(Circle&& other) noexcept : Shape(std::move(other))
   {
      std::cout << " [Move] Moved a Circle\n"; // Move constructor
   }

   void draw() const override
   {
      std::cout << " [Drawing] Circle at memory address: " << this << "\n";
   }
};

// Define the variant to hold shapes by value
using ShapeVariant = std::variant<Square, Circle>;

//------------------------------------------------------------------- Main:
int main()
{
   std::cout << "=== VARIANT & VALUE SEMANTICS: AUTOMATIC CLONING ===\n";

   std::vector<ShapeVariant> originals;
   std::vector<ShapeVariant> clones;

   std::cout << "\n--- PHASE 1: Creating new objects ---\n";
   originals.push_back(Square());
   originals.push_back(Circle());

   std::cout << "\n--- PHASE 2: Printing addresses in originals ---\n";
   for(const auto& v : originals) {
      std::visit([](const auto& shape) { shape.draw(); }, v);
   }

   std::cout << "\n--- PHASE 3: Copying into clones ---\n";
   for(const auto& v : originals)
      clones.push_back(v);

   std::cout << "\n--- PHASE 4: Printing addresses in clones ---\n";
   for (const auto& v : clones) {
      std::visit([](const auto& shape) { shape.draw(); }, v);
   }
   std::cout << " [Success] All objects cloned automatically via value semantics.\n"
                "           Compare the addresses above.\n";

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
