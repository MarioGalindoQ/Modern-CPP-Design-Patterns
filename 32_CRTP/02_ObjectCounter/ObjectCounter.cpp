/**
 * ============================================================================
 * File: ObjectCounter.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the "Object Counter" idiom using CRTP. It allows 
 * tracking the number of instances created and currently alive for any class.
 * 
 * --- THE POWER OF CRTP:
 * Because 'ObjectCounter<X>' and 'ObjectCounter<Y>' are distinct types, 
 * the compiler creates separate static variables for each. This ensures 
 * that the count for 'Dragon' is independent of the count for 'Elephant',
 * even though they share the same base logic.
 * ============================================================================
 */

#include <iostream>
#include <string>

//--------------------------------------------------------- Counter Base (CRTP):
template <class T>
class ObjectCounter
{
private:
   // C++17 inline static variables allow definition inside the header/class.
   static inline int createdCount_{0};
   static inline int aliveCount_{0};

protected:
   // Protected constructor: only derived classes can instantiate.
   ObjectCounter()
   {
      ++createdCount_;
      ++aliveCount_;
   }

   // Destructor decrements only the alive count.
   ~ObjectCounter()
   {
      --aliveCount_;
   }

public:
   static int getCreated() { return createdCount_; }
   static int getAlive()   { return aliveCount_; }
};

//------------------------------------------------------- Concrete Class A:
class Dragon : public ObjectCounter<Dragon>
{
public:
   void roar() const { std::cout << " [Dragon] ROAR!\n"; }
};

//------------------------------------------------------- Concrete Class B:
class Elephant : public ObjectCounter<Elephant>
{
public:
   void trumpet() const { std::cout << " [Elephant] TRUMPET!\n"; }
};

//----------------------------------------------------------- PrintSummary:
void printSummary()
{
   std::cout << " Dragons created: " << Dragon::getCreated()
             << ", alive: " << Dragon::getAlive() << "\n";

   std::cout << " Elephants created: " << Elephant::getCreated()
             << ", alive: " << Elephant::getAlive() << "\n\n";
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== CRTP: OBJECT COUNTER SIMULATION ===\n" << std::endl;

   std::cout << "--- Initial state ---\n";
   printSummary();

   // 1. Creating objects
   std::cout << "Creating 2 Dragons and 1 Elephant...\n";
   Dragon* d1 = new Dragon();
   Dragon* d2 = new Dragon();
   Elephant* e1 = new Elephant();
   printSummary();

   // 2. Testing destruction
   std::cout << "Destroying 1 Dragon...\n";
   delete d1;
   printSummary();

   // Cleanup remaining
   delete d2;
   delete e1;

   std::cout << "=== FINAL TOTALS ===\n";
   printSummary();

   std::cout << "=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
