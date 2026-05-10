/**
 * ============================================================================
 * File: Prototype_deducing.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program implements the Prototype pattern using C++23 "Deducing This".
 * 
 * --- THE ARCHITECTURAL EVOLUTION:
 * 1. Traditional (01): Every class (Circle, Square) must manually override 
 *    the 'clone()' method, leading to boilerplate and potential errors.
 * 2. CRTP (32/03): Automates cloning but introduces complex circular 
 *    template inheritance 'Cloneable<Derived>'.
 * 3. Deducing This (02): Automates cloning using a single, non-template 
 *    Mixin. The base class method captures the derived type automatically.
 *
 * --- TECHNICAL MECHANICS:
 * We use 'this auto self' (pass-by-value). When 'circle.clone()' is called:
 * - 'self' is deduced as 'Circle'.
 * - Because it's passed by value, a copy is automatically created.
 * - We then move this copy into a 'std::unique_ptr'.
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>

//--------------------------------------------------------- Prototype Mixin:
class SmartCloneable {
public:
   virtual ~SmartCloneable() = default;

   // C++23 Deducing This: captures the actual type (Base or Derived)
   // Using "this auto self" (pass by value) force the copy
   auto clone(this auto self) {
      return std::make_unique<decltype(self)>(self);
   }

   // Polymorphic entry point required to clone from a Base pointer
   virtual std::unique_ptr<class Base> clone_polymorphic() const = 0;
};

//------------------------------------------------------------------- Base:
class Base : public SmartCloneable {
public:
   Base() = default;
   Base(const Base&) = default;

   virtual ~Base() {
      std::cout << " [Cleanup] Base object destroyed.\n";
   }

   void print() const {
      std::cout << " -> Object type: Base\n";
   }

   // Implementation using the Deducing This helper
   std::unique_ptr<Base> clone_polymorphic() const override {
      return this->clone();
   }
};

//---------------------------------------------------------------- Derived:
class Derived : public Base {
public:
   Derived() = default;
   Derived(const Derived& other) : Base(other) {}

   ~Derived() override {
      std::cout << " [Cleanup] Derived object destroyed.\n";
   }

   void print() const {
      std::cout << " -> Object type: Derived\n";
   }

   // Even in Derived, we just call the same deducing clone
   std::unique_ptr<Base> clone_polymorphic() const override {
      return this->clone();
   }
};

//------------------------------------------------------------------- Main:
int main() {
   std::cout << "=== PROTOTYPE PATTERN SIMULATION ===\n";

   {
      std::cout << "\nCreating a Base prototype:\n";
      Base original;
      original.print();

      std::cout << "Cloning Base:\n";
      auto copy = original.clone_polymorphic();
      copy->print();

      std::cout << "--- Base objects going out of scope ---\n";
   } // Scope ends, destructors called

   std::cout << "\n------------------------------------------\n";

   {
      std::cout << "\nCreating a Derived prototype:\n";
      Derived original;
      original.print();

      std::cout << "Cloning Derived (via Base pointer in createClone):\n";
      // To simulate the base pointer behavior:
      Base* ptr = &original;
      auto copy = ptr->clone_polymorphic();
      static_cast<Derived*>(copy.get())->print();

      std::cout << "--- Derived objects going out of scope ---\n";
   } // Scope ends, destructors called

   std::cout << "\n=== END OF SIMULATION ===\n";
}

//================================================================================ END
