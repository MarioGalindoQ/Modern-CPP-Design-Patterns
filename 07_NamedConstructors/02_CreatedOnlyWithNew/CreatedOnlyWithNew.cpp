/**
 * ============================================================================
 * File: CreatedOnlyWithNew.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * By making all constructors private and exposing static creation methods, 
 * we force objects to always be created on the heap (via 'new' wrapped in 
 * a std::unique_ptr) and never on the stack.
 * ============================================================================
 */

#include <iostream>
#include <memory>

class Fred
{
private:
   int i_;

   // The constructors themselves are private:
   Fred() : i_{0} { }
   explicit Fred(int i) : i_{i} { }
   Fred(const Fred& other) : i_{other.i_} { }

public:
   ~Fred()
   {
      std::cout << " [Cleanup] Fred " << i_ << " destroyed (Memory freed automatically).\n";
   }

   // Named constructors returning safe smart pointers:
   // Note: std::make_unique cannot be used here because constructors are private.
   static std::unique_ptr<Fred> create()
   {
      return std::unique_ptr<Fred>(new Fred());
   }

   static std::unique_ptr<Fred> create(int i)
   {
      return std::unique_ptr<Fred>(new Fred(i));
   }

   static std::unique_ptr<Fred> create(const Fred& other)
   {
      return std::unique_ptr<Fred>(new Fred(other));
   }

   static std::unique_ptr<Fred> create(const std::unique_ptr<Fred>& other)
   {
      return std::unique_ptr<Fred>(new Fred(*other));
   }

   void talk() const
   {
      std::cout << "  -> Fred talking: i = " << i_ << '\n';
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== NAMED CONSTRUCTORS (FORCE HEAP ALLOCATION) ===\n" << std::endl;

   // If we try: Fred p; 
   // The compiler will throw an error (constructor is private).

   std::cout << "Creating Freds dynamically...\n";

   std::unique_ptr<Fred> f1 = Fred::create(1);
   f1->talk();

   auto f2 = Fred::create(2);
   f2->talk();

   std::cout << "\nCreating copies of Fred...\n";

   std::unique_ptr<Fred> f1Copy = Fred::create(*f1);
   f1Copy->talk();

   auto f2Copy = Fred::create(f2);
   f2Copy->talk();

   std::cout << "\n=== END OF MAIN (Smart pointers will trigger cleanup) ===\n";
}

//================================================================================ END
