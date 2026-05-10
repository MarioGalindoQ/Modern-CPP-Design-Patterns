/**
 * ============================================================================
 * File: Prototype_traditional.cpp (Traditional Implementation)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the classic GoF Prototype pattern, specifically 
 * implemented through what is known in C++ as the "Virtual Constructor" idiom.
 * 
 * --- THE PROBLEM:
 * In C++, constructors cannot be virtual. If you hold a pointer to a base 
 * class (e.g., 'Base*'), you cannot simply create a copy of the underlying 
 * object (which might be a 'Derived' type) without knowing its concrete 
 * class at the call site. This creates tight coupling and prevents the 
 * polymorphic duplication of objects.
 * 
 * --- THE SOLUTION (VIRTUAL CONSTRUCTOR):
 * The Prototype pattern delegates the responsibility of duplication to the 
 * objects themselves. By defining a virtual 'clone()' method in the base 
 * class and overriding it in every derived class, each object acts as its 
 * own factory. 
 * 
 * --- TECHNICAL MECHANICS:
 * 1. Polymorphic Copying: The 'clone()' method returns a new instance of 
 *    the same type as the object it is called on, but through a base pointer.
 * 2. Smart Pointers: We use 'std::unique_ptr' to ensure that the newly 
 *    cloned objects are managed via RAII, preventing memory leaks.
 * 3. Manual Overrides: This "Traditional" version requires every new 
 *    derived class to manually implement 'clone()'.
 * ============================================================================
 */

#include <iostream>
#include <memory>

struct Base
{
   virtual ~Base() 
   {
      std::cout << " [Cleanup] Base object destroyed.\n";
   }

   virtual void print() const 
   {
      std::cout << " -> Object type: Base\n";
   }

   // The Virtual Constructor (Prototype)
   virtual std::unique_ptr<Base> clone() const
   {
      return std::make_unique<Base>(*this); // Copy current object
   }
};

struct Derived : Base
{
   ~Derived() override 
   {
      std::cout << " [Cleanup] Derived object destroyed.\n";
   }

   // Correct syntax: 'const' goes before 'override'
   void print() const override 
   {
      std::cout << " -> Object type: Derived\n";
   }

   // Overriding clone to return a new Derived object.
   // Note: When using unique_ptr, the return type must match the base exactly.
   std::unique_ptr<Base> clone() const override
   {
      return std::make_unique<Derived>(*this); // Copy current derived object
   }
};

/**
 * Global helper function to demonstrate the Virtual Constructor.
 * It doesn't know the concrete type of 'b', but it clones it correctly.
 */
std::unique_ptr<Base> createClone(const Base* b)
{
   return b->clone(); // Calls the virtual clone() method
}

void printInfo(const Base* b)
{
   b->print(); // Calls the virtual print() method
}

int main()
{
   std::cout << "=== PROTOTYPE PATTERN SIMULATION ===\n\n";

   {
      std::cout << "Creating a Base prototype:\n";
      std::unique_ptr<Base> base_1 = std::make_unique<Base>();
      printInfo(base_1.get());
      
      std::cout << "Cloning Base:\n";
      auto base_2 = createClone(base_1.get()); 
      printInfo(base_2.get());
      std::cout << "--- Base objects going out of scope ---\n";
   } // Destructors are called automatically here

   std::cout << "\n------------------------------------------\n\n";

   {
      std::cout << "Creating a Derived prototype:\n";
      std::unique_ptr<Base> derived_1 = std::make_unique<Derived>();
      printInfo(derived_1.get());
      
      std::cout << "Cloning Derived (via Base pointer in createClone):\n";
      // This is the magic: createClone receives Base* but clones a Derived object.
      auto derived_2 = createClone(derived_1.get()); 
      printInfo(derived_2.get());
      std::cout << "--- Derived objects going out of scope ---\n";
   } // Destructors are called automatically here

   std::cout << "\n=== END OF SIMULATION ===\n";
}

//================================================================================ END
