/**
 * ============================================================================
 * File: Visitor_GoF.cpp
 * 
 * --- DESIGN OVERVIEW:
 * Visitor pattern implemented to solve the "double dispatch problem".
 * This implementation keeps the object structure (Visitable) decoupled from
 * the operations (Visitor).
 * 
 * --- RECOMPILATION ANALYSIS:
 * 1. Incrementing a new Visitor -> Neither existing Visitors nor Visited 
 *    classes need to be recompiled.
 * 2. Incrementing a new Visited -> Visited classes do not need to be 
 *    recompiled, BUT ALL existing Visitor classes MUST be recompiled!
 * ============================================================================
 */

#include "Visitor.h"
#include <vector>
#include <iostream>
#include <memory>

//--------------------------------------------------------- Family Forward Decls:
class Red;
class Blue;

//--------------------------------------------------------- Visitor Interface:
// Needs to know about Red and Blue, so it is defined here instead of the 
// generic header to prevent cyclic dependencies in the infrastructure.
class Visitor
{
protected:
   Visitor() = default;

public:
   virtual ~Visitor() = default;

   // Visitor depends upon Visited types. 
   // Default implementation throws an error to ensure safety.
   virtual void visit(Red&)  { throw dispatch_error(typeid(*this).name(), "Red"); } 
   virtual void visit(Blue&) { throw dispatch_error(typeid(*this).name(), "Blue"); } 
};

//--------------------------------------------------------- Dispatch Actions:
// These represent the exclusive logic for each specific combination.
class Triangle;
class Circle;

void redTriangle(Red&, Triangle&)   { std::cout << " -> Action: Exclusive Red Triangle function\n"; }
void redCircle(Red&, Circle&)       { std::cout << " -> Action: Exclusive Red Circle function\n"; }
void blueTriangle(Blue&, Triangle&) { std::cout << " -> Action: Exclusive Blue Triangle function\n"; }
void blueCircle(Blue&, Circle&)     { std::cout << " -> Action: Exclusive Blue Circle function\n"; }

//--------------------------------------------------------- Colors (Visitable):
class Red : public Visitable
{
public:
   // Every Visitable must include this line to perform the handshake
   void accept(Visitor& visitor) override { visitor.visit(*this); } 
};

class Blue : public Visitable
{
public:
   void accept(Visitor& visitor) override { visitor.visit(*this); }
};

//--------------------------------------------------------- Shapes (Visitors):
class Triangle : public Visitor
{
public:
   // Overrides are defined selectively or for all types
   void visit(Red& red)   override { redTriangle(red, *this); } 
   void visit(Blue& blue) override { blueTriangle(blue, *this); }
};

class Circle : public Visitor
{
public:
   void visit(Red& red) override { redCircle(red, *this); }
   
   // Note: Blue is intentionally NOT overridden to test the exception logic.
   // void visit(Blue& blue) override { blueCircle(blue, *this); }
};

//--------------------------------------------------------- Simulation Engine:
using Visitable_ptr = std::unique_ptr<Visitable>;
using Visitor_ptr = std::unique_ptr<Visitor>;

/**
 * dispatch_all_combinations:
 * Iterates through the collections and executes the double dispatch.
 * It demonstrates how the symmetry of double_dispatch() makes the 
 * call natural regardless of the order of objects.
 */
void dispatch_all_combinations(
   std::vector<Visitable_ptr>& colors, 
   std::vector<Visitor_ptr>& shapes)
{
   for (auto& visited : colors)
   {
      for (auto& visitor : shapes)
      {
         try
         {
            // Handshake starts here!
            double_dispatch(*visited, *visitor); 
         }
         catch (const dispatch_error& e)
         {
            std::cerr << " [System] " << e.what() << std::endl;
         }
      }
   }
}

//--------------------------------------------------------- Main Entry Point:
int main()
{
   std::cout << "=== VISITOR PATTERN (GOF CLASSIC SIMULATION) ===\n" << std::endl;

   // 1. Setup the Visitable collection (The "Colors")
   std::vector<Visitable_ptr> colors;
   colors.push_back(std::make_unique<Red>());
   colors.push_back(std::make_unique<Blue>());

   // 2. Setup the Visitor collection (The "Shapes")
   std::vector<Visitor_ptr> shapes;
   shapes.push_back(std::make_unique<Triangle>());
   shapes.push_back(std::make_unique<Circle>());

   // 3. Process the matrix of interactions
   dispatch_all_combinations(colors, shapes);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
