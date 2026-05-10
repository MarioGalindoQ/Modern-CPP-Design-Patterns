/**
 * ============================================================================
 * File: VariantHeapClone.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a hybrid approach: using std::variant for 
 * heterogeneous storage while maintaining objects on the heap with 
 * std::unique_ptr. This requires a custom cloning mechanism via CRTP.
 * 
 * --- THE PROBLEM:
 * std::variant can hold std::unique_ptr, but since unique_ptr is non-copyable, 
 * the entire variant becomes non-copyable. Standard value semantics cloning 
 * (clones = originals) will no longer compile.
 * 
 * --- THE SOLUTION (CRTP MIXIN + VISIT):
 * 1. We re-introduce the Cloneable<T> Mixin to provide a non-virtual clone() 
 *    method that returns a new std::unique_ptr<T>.
 * 2. We use std::visit during the cloning phase to dispatch the call to 
 *    the correct CRTP clone() implementation at compile-time.
 * 
 * --- ADVANTAGES:
 * 1. Performance: Zero virtual function overhead for cloning.
 * 2. Safety: No raw pointers or manual memory management.
 * 3. Architecture: Business logic (Shape) remains separate from memory 
 *    concerns (Cloneable).
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <variant>

//--------------------------------------------------------- Cloneable Mixin:
template <typename Concrete>
class Cloneable {
public:
   std::unique_ptr<Concrete> clone() const {
      return std::make_unique<Concrete>(static_cast<const Concrete&>(*this));
   }
};

//---------------------------------------------------- Abstract Base Shape:
class Shape // Base
{
public:
   virtual ~Shape() = default;
   virtual void draw() const = 0;
};

//-------------------------------------------------- Concrete Shape Square:
class Square : public Shape, public Cloneable<Square>
{
public:
   Square() // Constructor
   {
      std::cout << " [Constructor] Created a new Square on the Heap\n";
   }

   Square(const Square& other) : Shape(other) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copied a Square\n";
   }

   void draw() const override
   {
      std::cout << " [Drawing] Square at memory address: " << this << "\n";
   }
};

//-------------------------------------------------- Concrete Shape Circle:
class Circle : public Shape, public Cloneable<Circle>
{
public:
   Circle() // Constructor
   {
      std::cout << " [Constructor] Created a new Circle on the Heap\n";
   }

   Circle(const Circle& other) : Shape(other) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copied a Circle\n";
   }

   void draw() const override
   {
      std::cout << " [Drawing] Circle at memory address: " << this << "\n";
   }
};

// Define the variant to hold shapes by unique_ptr
using ShapeVariant = std::variant<std::unique_ptr<Square>, std::unique_ptr<Circle>>;

//------------------------------------------------------------------- Main:
int main()
{
   std::cout << "=== VARIANT & HEAP STORAGE: CRTP CLONING SYSTEM ===\n";

   std::vector<ShapeVariant> originals;
   std::vector<ShapeVariant> clones;

   std::cout << "\n--- PHASE 1: Creating new objects ---\n";
   originals.push_back(std::make_unique<Square>());
   originals.push_back(std::make_unique<Circle>());

   std::cout << "\n--- PHASE 2: Printing addresses in originals ---\n";
   for(const auto& v : originals) {
      std::visit([](const auto& shapePtr) { shapePtr->draw(); }, v);
   }

   std::cout << "\n--- PHASE 3: Copying into clones ---\n";
   // We must manually clone because unique_ptr cannot be copied.
   // std::visit finds the correct clone() method at compile-time.
   for(const auto& v : originals) {
      auto clonedVariant = std::visit([](const auto& shapePtr) -> ShapeVariant {
         return shapePtr->clone();
         // Note: The '-> ShapeVariant' forces all lambda paths to return the same type.
         // Alternatively, one could write:
         //return ShapeVariant(shapePtr->clone());
      }, v);
      clones.push_back(std::move(clonedVariant));
   }

   std::cout << "\n--- PHASE 4: Printing addresses in clones ---\n";
   for (const auto& v : clones) {
      std::visit([](const auto& shapePtr) { shapePtr->draw(); }, v);
   }
   std::cout << " [Success] All heap-allocated objects cloned via CRTP Mixin.\n"
                "           Compare the addresses above.\n";

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
