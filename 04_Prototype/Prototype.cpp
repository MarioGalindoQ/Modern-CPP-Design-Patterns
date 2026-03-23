#include <iostream>
#include <memory>

/**
 * Prototype Pattern (Virtual Constructor)
 * 
 * This example demonstrates how to clone objects polymorphically.
 * We use smart pointers (std::unique_ptr) to ensure memory is 
 * automatically managed.
 */

struct Base
{
   // Virtual destructor is mandatory to allow safe deletion of Derived objects 
   // through a Base pointer.
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
   // Destructor specifically for Derived class
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
      std::unique_ptr<Derived> derived_1 = std::make_unique<Derived>();
      printInfo(derived_1.get());
      
      std::cout << "Cloning Derived (via Base pointer in createClone):\n";
      // This is the magic: createClone receives Base* but clones a Derived object.
      auto derived_2 = createClone(derived_1.get()); 
      printInfo(derived_2.get());
      std::cout << "--- Derived objects going out of scope ---\n";
   } // Destructors are called automatically here

   std::cout << "\n=== END OF SIMULATION ===\n";
   return 0;
}

//================================================================================ END
