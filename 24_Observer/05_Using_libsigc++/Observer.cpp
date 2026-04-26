// **************************************************************************
// Observer.cpp - Main application demonstrating the Observer Pattern
// Author: Mario Galindo Queralt, Ph.D.
// **************************************************************************
// This program illustrates a modern implementation of the Observer pattern 
// using libsigc++ and C++14/23 smart pointers.
//
// Application Flow:
// 1. Instantiates Subjects ('Number') and Concrete Observers.
// 2. Uses std::make_unique for modern, safe memory management.
// 3. Establishes many-to-many connections between subjects and observers.
// 4. Demonstrates dynamic connection management (attaching/disconnecting).
//
// Technical Note:
// All observers inherit from sigc::trackable, ensuring that they are 
// automatically disconnected from subjects upon destruction, 
// preventing any invalid memory access.
// **************************************************************************

#include "Objects.h"
#include <memory> // Required for std::unique_ptr and make_unique

int main()
{
   Number number_1, number_2;

   // Create Observers using std::make_unique
   auto observer_1 = std::make_unique<DivObserver>(4);
   auto observer_2 = std::make_unique<DivObserver>(3);
   auto observer_3 = std::make_unique<ModObserver>(3);

   // When using unique_ptr, we pass the dereferenced content (*)
   // attach(Observer&) receives the reference and mem_fun handles the rest
   sigc::connection con_1 = number_1.attach(*observer_1);
   sigc::connection con_2 = number_1.attach(*observer_2);
   sigc::connection con_3 = number_1.attach(*observer_3);

   number_2.attach(*observer_1);
   number_2.attach(*observer_2);
   number_2.attach(*observer_3);

   std::cout << "With three observers (unique_ptr):\n";
   number_1.setVal(14);
   number_2.setVal(18);

   std::cout << "\nWith two observers (disconnecting con_2):\n";
   con_2.disconnect();
   number_1.setVal(14);

   std::cout << "\nWith one observer (re-attaching observer_2):\n";
   con_1.disconnect();
   con_3.disconnect();
   number_1.attach(*observer_2);
   number_1.setVal(14);

   // Upon exiting main, the unique_ptrs are automatically destroyed.
   // Thanks to sigc::trackable, there will be no invalid access attempts.
}
//================================================================================ END
