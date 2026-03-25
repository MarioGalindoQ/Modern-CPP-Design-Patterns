/**
 * ============================================================================
 * File: Visitor.cpp (Simple Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program solves the "Double Dispatch" problem using the Visitable/Visitor 
 * logic. In C++, virtual functions only resolve the type of the caller 
 * at runtime. To resolve the types of BOTH objects (e.g., in a collision), 
 * we need a second dispatch.
 * 
 * Logic:
 * 1. first->intersect(*second)  // Runtime call on 'first'
 * 2. second->intersect(*this)   // Runtime call on 'second' with 'this' 
 *                               // as a concrete type.
 * 
 * --- NOTE ON DUAL ROLE:
 * In this particular double dispatch scenario, every object acts as both 
 * the 'Visitable' (the receiver) and the 'Visitor' (the executor). To reflect 
 * this dual responsibility, the base interface is named 'Visitable_Visitor'.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <memory>

class Circle;
class Triangle;

//------------------------------------------------- Base Interface (Dual Role):
class Visitable_Visitor
{
public:
   virtual ~Visitable_Visitor() = default;
   
   // The entry point for the double dispatch
   virtual void intersect(const Visitable_Visitor& other) const = 0;
   
   // The overloaded verification methods for concrete types
   virtual void intersect(const Circle& other) const = 0;
   virtual void intersect(const Triangle& other) const = 0;
};

//--------------------------------------------------------- Concrete Shape A:
class Circle : public Visitable_Visitor
{
public:
   void intersect(const Visitable_Visitor& other) const override 
   {
      // First dispatch: 'other' is polymorphic, but '*this' is known as Circle.
      other.intersect(*this); 
   }

   void intersect(const Circle&) const override 
   {
      std::cout << " [Verification] Checking intersection: Circle <-> Circle\n";
   }

   void intersect(const Triangle&) const override 
   {
      std::cout << " [Verification] Checking intersection: Circle <-> Triangle\n";
   }
};

//--------------------------------------------------------- Concrete Shape B:
class Triangle : public Visitable_Visitor
{
public:
   void intersect(const Visitable_Visitor& other) const override 
   {
      // First dispatch: 'other' is polymorphic, but '*this' is known as Triangle.
      other.intersect(*this); 
   }

   void intersect(const Circle&) const override 
   {
      std::cout << " [Verification] Checking intersection: Triangle <-> Circle\n";
   }

   void intersect(const Triangle&) const override 
   {
      std::cout << " [Verification] Checking intersection: Triangle <-> Triangle\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== DOUBLE DISPATCH (VISITABLE_VISITOR SIMULATION) ===\n" << std::endl;

   // Setup a heterogeneous collection of shapes using the dual-role interface
   std::vector<std::unique_ptr<Visitable_Visitor>> shapes;
   shapes.push_back(std::make_unique<Triangle>());
   shapes.push_back(std::make_unique<Circle>());

   // Test every possible combination using nested loops.
   // The output will verify that the specific types were correctly identified.
   for (const auto& s1 : shapes)
   {
      for (const auto& s2 : shapes)
      {
         s1->intersect(*s2);
      }
   }

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
