/**
 * ============================================================================
 * File: Visitor_RTTI.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This implementation uses Runtime Type Information (RTTI) to allow creating 
 * new Visitable or Visitor classes without recompiling existing ones.
 * 
 * It solves the "Cyclic Dependency" of the GoF pattern by using dynamic_cast 
 * to verify interface compatibility at runtime.
 * ============================================================================
 */

#include "Visitor_RTTI.h"
#include <vector>
#include <iostream>
#include <memory>

//--------------------------------------------------------- Forward Decls:
class Red;
class Blue;
class Triangle;
class Circle;

//--------------------------------------------------------- Dispatch Actions:
void redTriangle(Red&, Triangle&)   { std::cout << " -> Exclusive RED + TRIANGLE function\n"; }
void redCircle(Red&, Circle&)       { std::cout << " -> Exclusive RED + CIRCLE function\n"; }
void blueTriangle(Blue&, Triangle&) { std::cout << " -> Exclusive BLUE + TRIANGLE function\n"; }
void blueCircle(Blue&, Circle&)     { std::cout << " -> Exclusive BLUE + CIRCLE function\n"; }

//--------------------------------------------------------- Colors (Visitable):
// Each class must inherit from 'Visitable' and the 'As<T>' helper.
class Red : public Visitable, public As<Red>
{
};

class Blue : public Visitable, public As<Blue>
{
};

//--------------------------------------------------------- Shapes (Visitor):
// Visitors inherit from 'Visitor' and any number of 'Visit<T>' interfaces.
class Triangle : public Visitor, 
                 public Visit<Red>, 
                 public Visit<Blue>
{
public:
   void visit(Red& red)   override { redTriangle(red, *this); }
   void visit(Blue& blue) override { blueTriangle(blue, *this); }
};

class Circle : public Visitor, 
               public Visit<Red>
{
public:
   void visit(Red& red) override { redCircle(red, *this); }
   
   // Blue is intentionally NOT implemented here to test Dispatch_error
};

//--------------------------------------------------------- Simulation Engine:
using Visitable_ptr = std::unique_ptr<Visitable>;
using Visitor_ptr = std::unique_ptr<Visitor>;

void dispatchAllCombinations(
   std::vector<Visitable_ptr>& colors, 
   std::vector<Visitor_ptr>& shapes)
{
   for (auto& visited : colors)
   {
      for (auto& visitor : shapes)
      {
         try
         {
            double_dispatch(*visited, *visitor);
         }
         catch (const Dispatch_error& e)
         {
            std::cerr << " [System] " << e.what() << std::endl;
         }
      }
   }
}

//--------------------------------------------------------- Main Entry Point:
int main()
{
   std::cout << "=== VISITOR PATTERN (RTTI / ACYCLIC VERSION) ===\n" << std::endl;

   std::vector<Visitable_ptr> colors;
   colors.push_back(std::make_unique<Red>());
   colors.push_back(std::make_unique<Blue>());

   std::vector<Visitor_ptr> shapes;
   shapes.push_back(std::make_unique<Triangle>());
   shapes.push_back(std::make_unique<Circle>());

   dispatchAllCombinations(colors, shapes);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
