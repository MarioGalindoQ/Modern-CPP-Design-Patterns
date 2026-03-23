/**
 * ============================================================================
 * File: StaticPolymorphism.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates Static Polymorphism using the CRTP idiom.
 * It simulates a printer system where the 'Printer' base class defines
 * the public interface, but the actual implementation is resolved at 
 * compile-time by downcasting to the 'Derived' template argument.
 * 
 * --- KEY ADVANTAGE:
 * We can write generic functions like 'executePrinter' that work with any 
 * future printer type without using Virtual Tables (vtables). The compiler 
 * generates optimized code for each specific printer type used.
 * ============================================================================
 */

#include <iostream>
#include <string>

//--------------------------------------------------------- Base Class (CRTP):
template <class Derived>
class Printer
{
public:
   // The public interface
   void print(const std::string& message)
   {
      std::cout << " [Base] Preparing to print...\n";
      
      // Static downcast: converting 'this' from Printer<Derived>* to Derived*
      // This is safe because of the CRTP inheritance structure.
      static_cast<Derived*>(this)->printImplementation(message);
   }
};

//------------------------------------------------------- Concrete Printer A:
class InkjetPrinter : public Printer<InkjetPrinter>
{
public:
   void printImplementation(const std::string& message)
   {
      std::cout << "  -> [Inkjet] Spraying ink: " << message << "\n";
   }
};

//------------------------------------------------------- Concrete Printer B:
class LaserPrinter : public Printer<LaserPrinter>
{
public:
   void printImplementation(const std::string& message)
   {
      std::cout << "  -> [Laser] Fusing toner: " << message << "\n";
   }
};

//------------------------------------------------- Client Code (Static Dispatch):
// This function is the "contract". It can be written before knowing 
// all concrete printer implementations.
template <class T>
void executePrinter(Printer<T>* printer, const std::string& msg)
{
   // Even through a base pointer, the correct derived method is called
   // at compile-time without virtual table overhead.
   printer->print(msg);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== CRTP: STATIC POLYMORPHISM SIMULATION ===\n" << std::endl;

   // 1. Using Inkjet Printer
   InkjetPrinter* inkjet = new InkjetPrinter();
   std::cout << "Testing Inkjet Printer:\n";
   executePrinter(inkjet, "Medium Performance Printing");

   std::cout << "\n";

   // 2. Using Laser Printer
   LaserPrinter* laser = new LaserPrinter();
   std::cout << "Testing Laser Printer:\n";
   executePrinter(laser, "High Performance Printing.");

   // Cleanup
   delete inkjet;
   delete laser;

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
