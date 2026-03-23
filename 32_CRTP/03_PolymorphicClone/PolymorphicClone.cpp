/**
 * ============================================================================
 * File: PolymorphicClone.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a highly sophisticated, non-intrusive implementation 
 * of the Prototype pattern using a combination of CRTP (Curiously Recurring 
 * Template Pattern), Multiple Inheritance, and Cross-Casting.
 * It demonstrates how CRTP can automate the Prototype pattern's 'clone()' 
 * method and also the power of Multiple Inheritance to inject the "Clonable" 
 * into a hierarchy with Mixin.
 * 
 * --- THE PROBLEM:
 * In traditional polymorphism, every derived class must manually implement 
 * its own 'clone()' method to return a copy of itself (see 04_Prototype). This
 * leads to redundant, error-prone boilerplate code and tightly couples the
 * business interface (e.g., 'Shape') to memory management concerns.
 *
 * --- THE SOLUTION (CRTP MIXIN):
 * We use a generic 'Cloneable<T>' Mixin that implements the 'clone()' logic 
 * once. By inheriting from this Mixin, concrete classes automatically gain 
 * polymorphic cloning capabilities without writing any additional code. 
 * This effectively automates the Prototype pattern.
 *
 * --- THE ARCHITECTURAL ADVANTAGE (NON-INTRUSIVE DESIGN):
 * We separate Business Logic from System Infrastructure. The 'Shape' 
 * interface remains 100% pure and unaware of cloning. This is achieved 
 * through C++ Multiple Inheritance, allowing a class like 'Square' to 
 * satisfy two independent hierarchies:
 *    1. Business Hierarchy: Square "is a" Shape (for drawing).
 *    2. System Hierarchy:   Square "is a" Cloneable object (for memory).
 * 
 * --- TECHNICAL MECHANICS (CROSS-CASTING):
 * The system uses 'dynamic_cast' to perform a "cross-cast" between these 
 * unrelated branches at runtime. When the client asks to clone a 'Shape', 
 * the infrastructure ('Cloneable.h') searches the object's hierarchy for the 
 * 'ICloneable' interface. If found, the CRTP-generated logic is executed.
 * 
 * This highlights a key advantage of C++ over languages like Java, as it 
 * allows the clean injection of generic behaviors into independent class 
 * hierarchies with zero overhead and absolute type safety.
 * 
 * --- ERROR HANDLING & SIMULATION:
 * The infrastructure follows a "Fail-Fast" policy. 
 * 1. Success Case: Cloning 'Square' and 'Circle' which implement the Mixin.
 * 2. Error Case: Attempting to clone a 'Triangle' which is a valid 'Shape' 
 *    but not 'Cloneable'. The system detects this at runtime and throws 
 *    an exception, preventing null pointer propagation and ensuring 
 *    architectural integrity.
 * ============================================================================
 */

#include "Cloneable.h"

#include <iostream>
#include <vector>
#include <string>

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
      std::cout << " [Constructor] Created a new Square\n";
   }

   Square(const Square&) // Copy constructor
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
      std::cout << " [Constructor] Created a new Circle\n";
   }

   Circle(const Circle&) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copied a Circle\n";
   }

   void draw() const override
   {
      std::cout << " [Drawing] Circle at memory address: " << this << "\n";
   }
};

//------------------------------------------------ Concrete Shape Triangle:
// This class is NOT cloneable (missing the Cloneable Mixin by mistake)
class Triangle : public Shape //, public Cloneable<Triangle>
{
public:
   Triangle() // Constructor
   {
      std::cout << " [Constructor] Created a new Triangle without the Cloneable Mixin\n";
   }

   Triangle(const Triangle&) // Copy constructor
   {
      std::cout << " [Copy Constructor] Copying a Triangle without the Cloneable Mixin\n";
   }

   void draw() const override
   {
      std::cout << " [Drawing] Triangle at memory address: " << this << " (Static member)\n";
   }
};

//------------------------------------------------------------------- Main:
int main()
{
   std::cout << "=== CRTP & CROSS-CASTING: SMART CLONING SYSTEM ===\n";

   std::vector<std::unique_ptr<Shape>> originals;
   std::vector<std::unique_ptr<Shape>> clones;

   std::cout << "\n--- PHASE 1: Creating new objects ---\n";
   originals.push_back(std::make_unique<Square>());
   originals.push_back(std::make_unique<Circle>());

   std::cout << "\n--- PHASE 2: Printing addresses in originals ---\n";
   for (const auto& shape : originals) shape->draw();

   try
   {
      std::cout << "\n--- PHASE 3: Copying into clones ---\n";
      for(const auto& shape : originals)
         clones.push_back(Cloneable_clone(shape.get()));

      std::cout << "\n--- PHASE 4: Printing addresses in clones ---\n";
      for (const auto& shape : clones) shape->draw();
      std::cout << " [Success] All compatible objects cloned. Compare the addresses above.\n";
   }
   catch(const std::exception& e)
   {
      std::cerr << " [Unexpected Error] " << e.what() << "\n";
   }

   std::cout << "\n--- PHASE 5: Not cloneable error detection ---\n";
   Triangle triangle;
   
   try
   {
      std::cout << " Attempting to clone the Triangle...\n";
      auto failed_clone = Cloneable_clone(&triangle);
   }
   catch (const std::exception& e)
   {
      std::cout << " [Caught Expected Error] " << e.what() << "\n";
   }

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
