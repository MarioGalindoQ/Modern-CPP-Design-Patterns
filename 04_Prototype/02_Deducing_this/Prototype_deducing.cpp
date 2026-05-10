/**
 * ============================================================================
 * File: Prototype_deducing.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program implements a cutting-edge version of the Prototype pattern 
 * using C++23 "Deducing This" (Explicit Object Parameters). It represents 
 * the pinnacle of the pattern's evolution in C++.
 * 
 * --- THE ARCHITECTURAL EVOLUTION (INTERFACE PATTERN):
 * 1. Infrastructure Layer (Mixin): 'SmartCloneable' is a non-template utility 
 *    that provides the static cloning machinery.
 * 
 * 2. Interface Layer (Base): 'Base' is now a pure Interface (ABC). It defines 
 *    the contract for the domain but cannot be instantiated. Crucially, 
 *    cloning a 'Base' directly is impossible because abstract types cannot 
 *    be created by the Mixin's 'make_unique' call.
 * 
 * 3. Implementation Layer (Derived): Concrete classes implement the interface 
 *    and leverage the Mixin to satisfy the virtual cloning contract with 
 *    zero boilerplate.
 *
 * --- TECHNICAL MECHANICS:
 * - Deducing This: 'clone(this auto self)' deduces the derived type at the 
 *   call site.
 * - Static Safety: Attempting to clone an abstract 'Base' results in a 
 *   compile-time error, as 'decltype(self)' would be an abstract type.
 * - Polymorphism: 'clone_polymorphic()' bridges the static machinery with 
 *   traditional dynamic dispatch.
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>

//--------------------------------------------------------- Prototype Mixin:
class SmartCloneable {
public:
   virtual ~SmartCloneable() = default;

   /**
    * The 'this auto self' parameter captures the concrete type.
    * If called on an abstract type, std::make_unique will fail to compile,
    * providing perfect architectural enforcement.
    * Guideline: "this auto self" (pass by value) ensures a deep copy is 
    * automatically created as a function parameter.
    */
   auto clone(this auto self) {
      return std::make_unique<decltype(self)>(std::move(self));
   }
};

//--------------------------------------------------------- Base Interface:
class Base : public SmartCloneable {
public:
   // Pure Interface: No data, only contract.
   virtual ~Base() {
      std::cout << " [Cleanup] Interface Base destroyed.\n";
   }

   virtual void print() const = 0; // Pure Virtual

   /**
    * The Virtual Constructor Contract:
    * Must be implemented by concrete classes.
    */
   virtual std::unique_ptr<Base> clone_polymorphic() const = 0;
};

//------------------------------------------------------- Concrete Derived:
class Derived : public Base {
public:
   Derived() = default;
   Derived(const Derived& other) : Base(other) {}

   ~Derived() override {
      std::cout << " [Cleanup] Concrete Derived destroyed.\n";
   }

   void print() const override {
      std::cout << " -> Object type: Concrete Derived\n";
   }

   /**
    * Implementation using the Mixin's static logic.
    * Returns a unique_ptr<Derived> which is then upcasted to unique_ptr<Base>.
    */
   std::unique_ptr<Base> clone_polymorphic() const override {
      return this->clone();
   }
};

//------------------------------------------------------------------- Main:
int main() {
   std::cout << "=== PROTOTYPE PATTERN SIMULATION: C++23 DEDUCING THIS ===\n";

   {
      std::cout << "\n--- PHASE 1: Polymorphic Cloning via Interface ---\n";
      // We cannot do: Base b; or b.clone(); -> It would not compile.
      
      std::unique_ptr<Base> original = std::make_unique<Derived>();
      std::cout << "Original object (managed via Interface pointer):\n";
      original->print();

      std::cout << "Cloning via virtual contract:\n";
      auto copy = original->clone_polymorphic();
      copy->print();

      std::cout << "--- Objects going out of scope ---\n";
   } 

   std::cout << "\n----------------------------------------------------------\n";

   {
      std::cout << "\n--- PHASE 2: Static Covariance (Direct Access) ---\n";
      Derived original;
      original.print();

      std::cout << "Cloning via Deducing This:\n";
      // Returns std::unique_ptr<Derived> directly.
      auto specificClone = original.clone();
      specificClone->print();

      std::cout << "--- Objects going out of scope ---\n";
   } 

   std::cout << "\n=== END OF SIMULATION ===\n";
}

//================================================================================ END
